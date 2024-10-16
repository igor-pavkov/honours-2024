static int nntp_msg_open(struct Context *ctx, struct Message *msg, int msgno)
{
  struct NntpData *nntp_data = ctx->data;
  struct Header *hdr = ctx->hdrs[msgno];
  char article[16];

  /* try to get article from cache */
  struct NntpAcache *acache = &nntp_data->acache[hdr->index % NNTP_ACACHE_LEN];
  if (acache->path)
  {
    if (acache->index == hdr->index)
    {
      msg->fp = mutt_file_fopen(acache->path, "r");
      if (msg->fp)
        return 0;
    }
    /* clear previous entry */
    else
    {
      unlink(acache->path);
      FREE(&acache->path);
    }
  }
  snprintf(article, sizeof(article), "%d", NHDR(hdr)->article_num);
  msg->fp = mutt_bcache_get(nntp_data->bcache, article);
  if (msg->fp)
  {
    if (NHDR(hdr)->parsed)
      return 0;
  }
  else
  {
    char buf[PATH_MAX];
    /* don't try to fetch article from removed newsgroup */
    if (nntp_data->deleted)
      return -1;

    /* create new cache file */
    const char *fetch_msg = _("Fetching message...");
    mutt_message(fetch_msg);
    msg->fp = mutt_bcache_put(nntp_data->bcache, article);
    if (!msg->fp)
    {
      mutt_mktemp(buf, sizeof(buf));
      acache->path = mutt_str_strdup(buf);
      acache->index = hdr->index;
      msg->fp = mutt_file_fopen(acache->path, "w+");
      if (!msg->fp)
      {
        mutt_perror(acache->path);
        unlink(acache->path);
        FREE(&acache->path);
        return -1;
      }
    }

    /* fetch message to cache file */
    snprintf(buf, sizeof(buf), "ARTICLE %s\r\n",
             NHDR(hdr)->article_num ? article : hdr->env->message_id);
    const int rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), fetch_msg,
                                    fetch_tempfile, msg->fp);
    if (rc)
    {
      mutt_file_fclose(&msg->fp);
      if (acache->path)
      {
        unlink(acache->path);
        FREE(&acache->path);
      }
      if (rc > 0)
      {
        if (mutt_str_strncmp(NHDR(hdr)->article_num ? "423" : "430", buf, 3) == 0)
        {
          mutt_error(_("Article %d not found on the server."),
                     NHDR(hdr)->article_num ? article : hdr->env->message_id);
        }
        else
          mutt_error("ARTICLE: %s", buf);
      }
      return -1;
    }

    if (!acache->path)
      mutt_bcache_commit(nntp_data->bcache, article);
  }

  /* replace envelope with new one
   * hash elements must be updated because pointers will be changed */
  if (ctx->id_hash && hdr->env->message_id)
    mutt_hash_delete(ctx->id_hash, hdr->env->message_id, hdr);
  if (ctx->subj_hash && hdr->env->real_subj)
    mutt_hash_delete(ctx->subj_hash, hdr->env->real_subj, hdr);

  mutt_env_free(&hdr->env);
  hdr->env = mutt_rfc822_read_header(msg->fp, hdr, 0, 0);

  if (ctx->id_hash && hdr->env->message_id)
    mutt_hash_insert(ctx->id_hash, hdr->env->message_id, hdr);
  if (ctx->subj_hash && hdr->env->real_subj)
    mutt_hash_insert(ctx->subj_hash, hdr->env->real_subj, hdr);

  /* fix content length */
  fseek(msg->fp, 0, SEEK_END);
  hdr->content->length = ftell(msg->fp) - hdr->content->offset;

  /* this is called in neomutt before the open which fetches the message,
   * which is probably wrong, but we just call it again here to handle
   * the problem instead of fixing it */
  NHDR(hdr)->parsed = true;
  mutt_parse_mime_message(ctx, hdr);

  /* these would normally be updated in mx_update_context(), but the
   * full headers aren't parsed with overview, so the information wasn't
   * available then */
  if (WithCrypto)
    hdr->security = crypt_query(hdr->content);

  rewind(msg->fp);
  mutt_clear_error();
  return 0;
}
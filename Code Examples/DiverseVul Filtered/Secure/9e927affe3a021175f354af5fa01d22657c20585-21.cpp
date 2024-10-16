static int check_mailbox(struct Context *ctx)
{
  struct NntpData *nntp_data = ctx->data;
  struct NntpServer *nserv = nntp_data->nserv;
  time_t now = time(NULL);
  int rc, ret = 0;
  void *hc = NULL;

  if (nserv->check_time + NntpPoll > now)
    return 0;

  mutt_message(_("Checking for new messages..."));
  if (nntp_newsrc_parse(nserv) < 0)
    return -1;

  nserv->check_time = now;
  rc = nntp_group_poll(nntp_data, 0);
  if (rc < 0)
  {
    nntp_newsrc_close(nserv);
    return -1;
  }
  if (rc)
    nntp_active_save_cache(nserv);

  /* articles have been renumbered, remove all headers */
  if (nntp_data->last_message < nntp_data->last_loaded)
  {
    for (int i = 0; i < ctx->msgcount; i++)
      mutt_header_free(&ctx->hdrs[i]);
    ctx->msgcount = 0;
    ctx->tagged = 0;

    if (nntp_data->last_message < nntp_data->last_loaded)
    {
      nntp_data->last_loaded = nntp_data->first_message - 1;
      if (NntpContext && nntp_data->last_message - nntp_data->last_loaded > NntpContext)
        nntp_data->last_loaded = nntp_data->last_message - NntpContext;
    }
    ret = MUTT_REOPENED;
  }

  /* .newsrc has been externally modified */
  if (nserv->newsrc_modified)
  {
#ifdef USE_HCACHE
    unsigned char *messages = NULL;
    char buf[16];
    void *hdata = NULL;
    struct Header *hdr = NULL;
    anum_t first = nntp_data->first_message;

    if (NntpContext && nntp_data->last_message - first + 1 > NntpContext)
      first = nntp_data->last_message - NntpContext + 1;
    messages = mutt_mem_calloc(nntp_data->last_loaded - first + 1, sizeof(unsigned char));
    hc = nntp_hcache_open(nntp_data);
    nntp_hcache_update(nntp_data, hc);
#endif

    /* update flags according to .newsrc */
    int j = 0;
    anum_t anum;
    for (int i = 0; i < ctx->msgcount; i++)
    {
      bool flagged = false;
      anum = NHDR(ctx->hdrs[i])->article_num;

#ifdef USE_HCACHE
      /* check hcache for flagged and deleted flags */
      if (hc)
      {
        if (anum >= first && anum <= nntp_data->last_loaded)
          messages[anum - first] = 1;

        snprintf(buf, sizeof(buf), "%u", anum);
        hdata = mutt_hcache_fetch(hc, buf, strlen(buf));
        if (hdata)
        {
          bool deleted;

          mutt_debug(2, "#1 mutt_hcache_fetch %s\n", buf);
          hdr = mutt_hcache_restore(hdata);
          mutt_hcache_free(hc, &hdata);
          hdr->data = 0;
          deleted = hdr->deleted;
          flagged = hdr->flagged;
          mutt_header_free(&hdr);

          /* header marked as deleted, removing from context */
          if (deleted)
          {
            mutt_set_flag(ctx, ctx->hdrs[i], MUTT_TAG, 0);
            mutt_header_free(&ctx->hdrs[i]);
            continue;
          }
        }
      }
#endif

      if (!ctx->hdrs[i]->changed)
      {
        ctx->hdrs[i]->flagged = flagged;
        ctx->hdrs[i]->read = false;
        ctx->hdrs[i]->old = false;
        nntp_article_status(ctx, ctx->hdrs[i], NULL, anum);
        if (!ctx->hdrs[i]->read)
          nntp_parse_xref(ctx, ctx->hdrs[i]);
      }
      ctx->hdrs[j++] = ctx->hdrs[i];
    }

#ifdef USE_HCACHE
    ctx->msgcount = j;

    /* restore headers without "deleted" flag */
    for (anum = first; anum <= nntp_data->last_loaded; anum++)
    {
      if (messages[anum - first])
        continue;

      snprintf(buf, sizeof(buf), "%u", anum);
      hdata = mutt_hcache_fetch(hc, buf, strlen(buf));
      if (hdata)
      {
        mutt_debug(2, "#2 mutt_hcache_fetch %s\n", buf);
        if (ctx->msgcount >= ctx->hdrmax)
          mx_alloc_memory(ctx);

        ctx->hdrs[ctx->msgcount] = hdr = mutt_hcache_restore(hdata);
        mutt_hcache_free(hc, &hdata);
        hdr->data = 0;
        if (hdr->deleted)
        {
          mutt_header_free(&hdr);
          if (nntp_data->bcache)
          {
            mutt_debug(2, "mutt_bcache_del %s\n", buf);
            mutt_bcache_del(nntp_data->bcache, buf);
          }
          continue;
        }

        ctx->msgcount++;
        hdr->read = false;
        hdr->old = false;
        hdr->data = mutt_mem_calloc(1, sizeof(struct NntpHeaderData));
        NHDR(hdr)->article_num = anum;
        nntp_article_status(ctx, hdr, NULL, anum);
        if (!hdr->read)
          nntp_parse_xref(ctx, hdr);
      }
    }
    FREE(&messages);
#endif

    nserv->newsrc_modified = false;
    ret = MUTT_REOPENED;
  }

  /* some headers were removed, context must be updated */
  if (ret == MUTT_REOPENED)
  {
    if (ctx->subj_hash)
      mutt_hash_destroy(&ctx->subj_hash);
    if (ctx->id_hash)
      mutt_hash_destroy(&ctx->id_hash);
    mutt_clear_threads(ctx);

    ctx->vcount = 0;
    ctx->deleted = 0;
    ctx->new = 0;
    ctx->unread = 0;
    ctx->flagged = 0;
    ctx->changed = false;
    ctx->id_hash = NULL;
    ctx->subj_hash = NULL;
    mx_update_context(ctx, ctx->msgcount);
  }

  /* fetch headers of new articles */
  if (nntp_data->last_message > nntp_data->last_loaded)
  {
    int oldmsgcount = ctx->msgcount;
    bool quiet = ctx->quiet;
    ctx->quiet = true;
#ifdef USE_HCACHE
    if (!hc)
    {
      hc = nntp_hcache_open(nntp_data);
      nntp_hcache_update(nntp_data, hc);
    }
#endif
    rc = nntp_fetch_headers(ctx, hc, nntp_data->last_loaded + 1, nntp_data->last_message, 0);
    ctx->quiet = quiet;
    if (rc >= 0)
      nntp_data->last_loaded = nntp_data->last_message;
    if (ret == 0 && ctx->msgcount > oldmsgcount)
      ret = MUTT_NEW_MAIL;
  }

#ifdef USE_HCACHE
  mutt_hcache_close(hc);
#endif
  if (ret)
    nntp_newsrc_close(nserv);
  mutt_clear_error();
  return ret;
}
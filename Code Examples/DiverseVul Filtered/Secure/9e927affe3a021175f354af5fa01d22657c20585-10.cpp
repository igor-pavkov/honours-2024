static int parse_overview_line(char *line, void *data)
{
  struct FetchCtx *fc = data;
  struct Context *ctx = fc->ctx;
  struct NntpData *nntp_data = ctx->data;
  struct Header *hdr = NULL;
  char *header = NULL, *field = NULL;
  bool save = true;
  anum_t anum;

  if (!line)
    return 0;

  /* parse article number */
  field = strchr(line, '\t');
  if (field)
    *field++ = '\0';
  if (sscanf(line, ANUM, &anum) != 1)
    return 0;
  mutt_debug(2, "" ANUM "\n", anum);

  /* out of bounds */
  if (anum < fc->first || anum > fc->last)
    return 0;

  /* not in LISTGROUP */
  if (!fc->messages[anum - fc->first])
  {
    /* progress */
    if (!ctx->quiet)
      mutt_progress_update(&fc->progress, anum - fc->first + 1, -1);
    return 0;
  }

  /* convert overview line to header */
  FILE *fp = mutt_file_mkstemp();
  if (!fp)
    return -1;

  header = nntp_data->nserv->overview_fmt;
  while (field)
  {
    char *b = field;

    if (*header)
    {
      if (strstr(header, ":full") == NULL && fputs(header, fp) == EOF)
      {
        mutt_file_fclose(&fp);
        return -1;
      }
      header = strchr(header, '\0') + 1;
    }

    field = strchr(field, '\t');
    if (field)
      *field++ = '\0';
    if (fputs(b, fp) == EOF || fputc('\n', fp) == EOF)
    {
      mutt_file_fclose(&fp);
      return -1;
    }
  }
  rewind(fp);

  /* allocate memory for headers */
  if (ctx->msgcount >= ctx->hdrmax)
    mx_alloc_memory(ctx);

  /* parse header */
  hdr = ctx->hdrs[ctx->msgcount] = mutt_header_new();
  hdr->env = mutt_rfc822_read_header(fp, hdr, 0, 0);
  hdr->env->newsgroups = mutt_str_strdup(nntp_data->group);
  hdr->received = hdr->date_sent;
  mutt_file_fclose(&fp);

#ifdef USE_HCACHE
  if (fc->hc)
  {
    char buf[16];

    /* try to replace with header from cache */
    snprintf(buf, sizeof(buf), "%u", anum);
    void *hdata = mutt_hcache_fetch(fc->hc, buf, strlen(buf));
    if (hdata)
    {
      mutt_debug(2, "mutt_hcache_fetch %s\n", buf);
      mutt_header_free(&hdr);
      ctx->hdrs[ctx->msgcount] = hdr = mutt_hcache_restore(hdata);
      mutt_hcache_free(fc->hc, &hdata);
      hdr->data = 0;
      hdr->read = false;
      hdr->old = false;

      /* skip header marked as deleted in cache */
      if (hdr->deleted && !fc->restore)
      {
        if (nntp_data->bcache)
        {
          mutt_debug(2, "mutt_bcache_del %s\n", buf);
          mutt_bcache_del(nntp_data->bcache, buf);
        }
        save = false;
      }
    }

    /* not cached yet, store header */
    else
    {
      mutt_debug(2, "mutt_hcache_store %s\n", buf);
      mutt_hcache_store(fc->hc, buf, strlen(buf), hdr, 0);
    }
  }
#endif

  if (save)
  {
    hdr->index = ctx->msgcount++;
    hdr->read = false;
    hdr->old = false;
    hdr->deleted = false;
    hdr->data = mutt_mem_calloc(1, sizeof(struct NntpHeaderData));
    NHDR(hdr)->article_num = anum;
    if (fc->restore)
      hdr->changed = true;
    else
    {
      nntp_article_status(ctx, hdr, NULL, anum);
      if (!hdr->read)
        nntp_parse_xref(ctx, hdr);
    }
    if (anum > nntp_data->last_loaded)
      nntp_data->last_loaded = anum;
  }
  else
    mutt_header_free(&hdr);

  /* progress */
  if (!ctx->quiet)
    mutt_progress_update(&fc->progress, anum - fc->first + 1, -1);
  return 0;
}
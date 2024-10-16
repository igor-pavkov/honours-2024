static int nntp_mbox_sync(struct Context *ctx, int *index_hint)
{
  struct NntpData *nntp_data = ctx->data;
  int rc;
#ifdef USE_HCACHE
  header_cache_t *hc = NULL;
#endif

  /* check for new articles */
  nntp_data->nserv->check_time = 0;
  rc = check_mailbox(ctx);
  if (rc)
    return rc;

#ifdef USE_HCACHE
  nntp_data->last_cached = 0;
  hc = nntp_hcache_open(nntp_data);
#endif

  for (int i = 0; i < ctx->msgcount; i++)
  {
    struct Header *hdr = ctx->hdrs[i];
    char buf[16];

    snprintf(buf, sizeof(buf), "%d", NHDR(hdr)->article_num);
    if (nntp_data->bcache && hdr->deleted)
    {
      mutt_debug(2, "mutt_bcache_del %s\n", buf);
      mutt_bcache_del(nntp_data->bcache, buf);
    }

#ifdef USE_HCACHE
    if (hc && (hdr->changed || hdr->deleted))
    {
      if (hdr->deleted && !hdr->read)
        nntp_data->unread--;
      mutt_debug(2, "mutt_hcache_store %s\n", buf);
      mutt_hcache_store(hc, buf, strlen(buf), hdr, 0);
    }
#endif
  }

#ifdef USE_HCACHE
  if (hc)
  {
    mutt_hcache_close(hc);
    nntp_data->last_cached = nntp_data->last_loaded;
  }
#endif

  /* save .newsrc entries */
  nntp_newsrc_gen_entries(ctx);
  nntp_newsrc_update(nntp_data->nserv);
  nntp_newsrc_close(nntp_data->nserv);
  return 0;
}
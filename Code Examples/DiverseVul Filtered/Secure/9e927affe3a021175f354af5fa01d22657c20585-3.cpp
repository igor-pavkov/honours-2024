int nntp_check_children(struct Context *ctx, const char *msgid)
{
  struct NntpData *nntp_data = ctx->data;
  struct ChildCtx cc;
  char buf[STRING];
  int rc;
  bool quiet;
  void *hc = NULL;

  if (!nntp_data || !nntp_data->nserv)
    return -1;
  if (nntp_data->first_message > nntp_data->last_loaded)
    return 0;

  /* init context */
  cc.ctx = ctx;
  cc.num = 0;
  cc.max = 10;
  cc.child = mutt_mem_malloc(sizeof(anum_t) * cc.max);

  /* fetch numbers of child messages */
  snprintf(buf, sizeof(buf), "XPAT References %u-%u *%s*\r\n",
           nntp_data->first_message, nntp_data->last_loaded, msgid);
  rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), NULL, fetch_children, &cc);
  if (rc)
  {
    FREE(&cc.child);
    if (rc > 0)
    {
      if (mutt_str_strncmp("500", buf, 3) != 0)
        mutt_error("XPAT: %s", buf);
      else
      {
        mutt_error(_("Unable to find child articles because server does not "
                     "support XPAT command."));
      }
    }
    return -1;
  }

  /* fetch all found messages */
  quiet = ctx->quiet;
  ctx->quiet = true;
#ifdef USE_HCACHE
  hc = nntp_hcache_open(nntp_data);
#endif
  for (int i = 0; i < cc.num; i++)
  {
    rc = nntp_fetch_headers(ctx, hc, cc.child[i], cc.child[i], 1);
    if (rc < 0)
      break;
  }
#ifdef USE_HCACHE
  mutt_hcache_close(hc);
#endif
  ctx->quiet = quiet;
  FREE(&cc.child);
  return (rc < 0) ? -1 : 0;
}
static int nntp_mbox_open(struct Context *ctx)
{
  struct NntpServer *nserv = NULL;
  struct NntpData *nntp_data = NULL;
  char buf[HUGE_STRING];
  char server[LONG_STRING];
  char *group = NULL;
  int rc;
  void *hc = NULL;
  anum_t first, last, count = 0;
  struct Url url;

  mutt_str_strfcpy(buf, ctx->path, sizeof(buf));
  if (url_parse(&url, buf) < 0 || !url.host || !url.path ||
      !(url.scheme == U_NNTP || url.scheme == U_NNTPS))
  {
    url_free(&url);
    mutt_error(_("%s is an invalid newsgroup specification!"), ctx->path);
    return -1;
  }

  group = url.path;
  url.path = strchr(url.path, '\0');
  url_tostring(&url, server, sizeof(server), 0);
  nserv = nntp_select_server(server, true);
  url_free(&url);
  if (!nserv)
    return -1;
  CurrentNewsSrv = nserv;

  /* find news group data structure */
  nntp_data = mutt_hash_find(nserv->groups_hash, group);
  if (!nntp_data)
  {
    nntp_newsrc_close(nserv);
    mutt_error(_("Newsgroup %s not found on the server."), group);
    return -1;
  }

  mutt_bit_unset(ctx->rights, MUTT_ACL_INSERT);
  if (!nntp_data->newsrc_ent && !nntp_data->subscribed && !SaveUnsubscribed)
    ctx->readonly = true;

  /* select newsgroup */
  mutt_message(_("Selecting %s..."), group);
  buf[0] = '\0';
  if (nntp_query(nntp_data, buf, sizeof(buf)) < 0)
  {
    nntp_newsrc_close(nserv);
    return -1;
  }

  /* newsgroup not found, remove it */
  if (mutt_str_strncmp("411", buf, 3) == 0)
  {
    mutt_error(_("Newsgroup %s has been removed from the server."), nntp_data->group);
    if (!nntp_data->deleted)
    {
      nntp_data->deleted = true;
      nntp_active_save_cache(nserv);
    }
    if (nntp_data->newsrc_ent && !nntp_data->subscribed && !SaveUnsubscribed)
    {
      FREE(&nntp_data->newsrc_ent);
      nntp_data->newsrc_len = 0;
      nntp_delete_group_cache(nntp_data);
      nntp_newsrc_update(nserv);
    }
  }

  /* parse newsgroup info */
  else
  {
    if (sscanf(buf, "211 " ANUM " " ANUM " " ANUM, &count, &first, &last) != 3)
    {
      nntp_newsrc_close(nserv);
      mutt_error("GROUP: %s", buf);
      return -1;
    }
    nntp_data->first_message = first;
    nntp_data->last_message = last;
    nntp_data->deleted = false;

    /* get description if empty */
    if (NntpLoadDescription && !nntp_data->desc)
    {
      if (get_description(nntp_data, NULL, NULL) < 0)
      {
        nntp_newsrc_close(nserv);
        return -1;
      }
      if (nntp_data->desc)
        nntp_active_save_cache(nserv);
    }
  }

  time(&nserv->check_time);
  ctx->data = nntp_data;
  if (!nntp_data->bcache && (nntp_data->newsrc_ent || nntp_data->subscribed || SaveUnsubscribed))
    nntp_data->bcache = mutt_bcache_open(&nserv->conn->account, nntp_data->group);

  /* strip off extra articles if adding context is greater than $nntp_context */
  first = nntp_data->first_message;
  if (NntpContext && nntp_data->last_message - first + 1 > NntpContext)
    first = nntp_data->last_message - NntpContext + 1;
  nntp_data->last_loaded = first ? first - 1 : 0;
  count = nntp_data->first_message;
  nntp_data->first_message = first;
  nntp_bcache_update(nntp_data);
  nntp_data->first_message = count;
#ifdef USE_HCACHE
  hc = nntp_hcache_open(nntp_data);
  nntp_hcache_update(nntp_data, hc);
#endif
  if (!hc)
  {
    mutt_bit_unset(ctx->rights, MUTT_ACL_WRITE);
    mutt_bit_unset(ctx->rights, MUTT_ACL_DELETE);
  }
  nntp_newsrc_close(nserv);
  rc = nntp_fetch_headers(ctx, hc, first, nntp_data->last_message, 0);
#ifdef USE_HCACHE
  mutt_hcache_close(hc);
#endif
  if (rc < 0)
    return -1;
  nntp_data->last_loaded = nntp_data->last_message;
  nserv->newsrc_modified = false;
  return 0;
}
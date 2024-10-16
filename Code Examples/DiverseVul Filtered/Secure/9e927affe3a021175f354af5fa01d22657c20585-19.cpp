static int nntp_mbox_close(struct Context *ctx)
{
  struct NntpData *nntp_data = ctx->data, *nntp_tmp = NULL;

  if (!nntp_data)
    return 0;

  nntp_data->unread = ctx->unread;

  nntp_acache_free(nntp_data);
  if (!nntp_data->nserv || !nntp_data->nserv->groups_hash || !nntp_data->group)
    return 0;

  nntp_tmp = mutt_hash_find(nntp_data->nserv->groups_hash, nntp_data->group);
  if (nntp_tmp == NULL || nntp_tmp != nntp_data)
    nntp_data_free(nntp_data);
  return 0;
}
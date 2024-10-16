static int nntp_mbox_check(struct Context *ctx, int *index_hint)
{
  int ret = check_mailbox(ctx);
  if (ret == 0)
  {
    struct NntpData *nntp_data = ctx->data;
    struct NntpServer *nserv = nntp_data->nserv;
    nntp_newsrc_close(nserv);
  }
  return ret;
}
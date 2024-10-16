static int nntp_connect_error(struct NntpServer *nserv)
{
  nserv->status = NNTP_NONE;
  mutt_error(_("Server closed connection!"));
  return -1;
}
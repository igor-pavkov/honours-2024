static int get_description(struct NntpData *nntp_data, char *wildmat, char *msg)
{
  char buf[STRING];
  char *cmd = NULL;

  /* get newsgroup description, if possible */
  struct NntpServer *nserv = nntp_data->nserv;
  if (!wildmat)
    wildmat = nntp_data->group;
  if (nserv->hasLIST_NEWSGROUPS)
    cmd = "LIST NEWSGROUPS";
  else if (nserv->hasXGTITLE)
    cmd = "XGTITLE";
  else
    return 0;

  snprintf(buf, sizeof(buf), "%s %s\r\n", cmd, wildmat);
  int rc = nntp_fetch_lines(nntp_data, buf, sizeof(buf), msg, fetch_description, nserv);
  if (rc > 0)
  {
    mutt_error("%s: %s", cmd, buf);
  }
  return rc;
}
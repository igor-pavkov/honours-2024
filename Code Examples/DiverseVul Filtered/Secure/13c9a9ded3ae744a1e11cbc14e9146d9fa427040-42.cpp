static CURLcode imap_continue_authenticate(struct connectdata *conn,
                                           const char *resp)
{
  struct imap_conn *imapc = &conn->proto.imapc;

  return Curl_pp_sendf(&imapc->pp, "%s", resp);
}
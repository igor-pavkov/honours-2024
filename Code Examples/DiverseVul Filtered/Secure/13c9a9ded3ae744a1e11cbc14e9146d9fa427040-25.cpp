static CURLcode imap_multi_statemach(struct connectdata *conn, bool *done)
{
  CURLcode result = CURLE_OK;
  struct imap_conn *imapc = &conn->proto.imapc;

  if((conn->handler->flags & PROTOPT_SSL) && !imapc->ssldone) {
    result = Curl_ssl_connect_nonblocking(conn, FIRSTSOCKET, &imapc->ssldone);
    if(result || !imapc->ssldone)
      return result;
  }

  result = Curl_pp_statemach(&imapc->pp, FALSE);
  *done = (imapc->state == IMAP_STOP) ? TRUE : FALSE;

  return result;
}
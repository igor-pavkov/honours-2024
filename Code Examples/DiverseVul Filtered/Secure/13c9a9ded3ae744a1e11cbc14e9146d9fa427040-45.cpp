static CURLcode imap_block_statemach(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct imap_conn *imapc = &conn->proto.imapc;

  while(imapc->state != IMAP_STOP && !result)
    result = Curl_pp_statemach(&imapc->pp, TRUE);

  return result;
}
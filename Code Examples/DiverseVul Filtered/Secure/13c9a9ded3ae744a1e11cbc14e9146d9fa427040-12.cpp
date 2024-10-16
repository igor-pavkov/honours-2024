static CURLcode imap_dophase_done(struct connectdata *conn, bool connected)
{
  struct IMAP *imap = conn->data->req.protop;

  (void)connected;

  if(imap->transfer != FTPTRANSFER_BODY)
    /* no data to transfer */
    Curl_setup_transfer(conn, -1, -1, FALSE, NULL, -1, NULL);

  return CURLE_OK;
}
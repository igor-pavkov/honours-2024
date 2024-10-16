static CURLcode imap_state_servergreet_resp(struct connectdata *conn,
                                            int imapcode,
                                            imapstate instate)
{
  struct Curl_easy *data = conn->data;
  (void)instate; /* no use for this yet */

  if(imapcode == IMAP_RESP_PREAUTH) {
    /* PREAUTH */
    struct imap_conn *imapc = &conn->proto.imapc;
    imapc->preauth = TRUE;
    infof(data, "PREAUTH connection, already authenticated!\n");
  }
  else if(imapcode != IMAP_RESP_OK) {
    failf(data, "Got unexpected imap-server response");
    return CURLE_WEIRD_SERVER_REPLY;
  }

  return imap_perform_capability(conn);
}
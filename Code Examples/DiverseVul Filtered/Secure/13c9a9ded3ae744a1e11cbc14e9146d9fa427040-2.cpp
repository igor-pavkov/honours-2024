static CURLcode imap_state_fetch_final_resp(struct connectdata *conn,
                                            int imapcode,
                                            imapstate instate)
{
  CURLcode result = CURLE_OK;

  (void)instate; /* No use for this yet */

  if(imapcode != IMAP_RESP_OK)
    result = CURLE_WEIRD_SERVER_REPLY;
  else
    /* End of DONE phase */
    state(conn, IMAP_STOP);

  return result;
}
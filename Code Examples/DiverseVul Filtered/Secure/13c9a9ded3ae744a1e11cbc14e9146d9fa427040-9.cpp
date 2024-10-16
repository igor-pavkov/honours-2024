static CURLcode imap_state_listsearch_resp(struct connectdata *conn,
                                           int imapcode,
                                           imapstate instate)
{
  CURLcode result = CURLE_OK;
  char *line = conn->data->state.buffer;
  size_t len = strlen(line);

  (void)instate; /* No use for this yet */

  if(imapcode == '*') {
    /* Temporarily add the LF character back and send as body to the client */
    line[len] = '\n';
    result = Curl_client_write(conn, CLIENTWRITE_BODY, line, len + 1);
    line[len] = '\0';
  }
  else if(imapcode != IMAP_RESP_OK)
    result = CURLE_QUOTE_ERROR; /* TODO: Fix error code */
  else
    /* End of DO phase */
    state(conn, IMAP_STOP);

  return result;
}
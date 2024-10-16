static CURLcode imap_state_select_resp(struct connectdata *conn, int imapcode,
                                       imapstate instate)
{
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  struct IMAP *imap = conn->data->req.protop;
  struct imap_conn *imapc = &conn->proto.imapc;
  const char *line = data->state.buffer;
  char tmp[20];

  (void)instate; /* no use for this yet */

  if(imapcode == '*') {
    /* See if this is an UIDVALIDITY response */
    if(sscanf(line + 2, "OK [UIDVALIDITY %19[0123456789]]", tmp) == 1) {
      Curl_safefree(imapc->mailbox_uidvalidity);
      imapc->mailbox_uidvalidity = strdup(tmp);
    }
  }
  else if(imapcode == IMAP_RESP_OK) {
    /* Check if the UIDVALIDITY has been specified and matches */
    if(imap->uidvalidity && imapc->mailbox_uidvalidity &&
       strcmp(imap->uidvalidity, imapc->mailbox_uidvalidity)) {
      failf(conn->data, "Mailbox UIDVALIDITY has changed");
      result = CURLE_REMOTE_FILE_NOT_FOUND;
    }
    else {
      /* Note the currently opened mailbox on this connection */
      imapc->mailbox = strdup(imap->mailbox);

      if(imap->custom)
        result = imap_perform_list(conn);
      else if(imap->query)
        result = imap_perform_search(conn);
      else
        result = imap_perform_fetch(conn);
    }
  }
  else {
    failf(data, "Select failed");
    result = CURLE_LOGIN_DENIED;
  }

  return result;
}
static CURLcode imap_perform_list(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  struct IMAP *imap = data->req.protop;
  char *mailbox;

  if(imap->custom)
    /* Send the custom request */
    result = imap_sendf(conn, "%s%s", imap->custom,
                        imap->custom_params ? imap->custom_params : "");
  else {
    /* Make sure the mailbox is in the correct atom format if necessary */
    mailbox = imap->mailbox ? imap_atom(imap->mailbox, true) : strdup("");
    if(!mailbox)
      return CURLE_OUT_OF_MEMORY;

    /* Send the LIST command */
    result = imap_sendf(conn, "LIST \"%s\" *", mailbox);

    free(mailbox);
  }

  if(!result)
    state(conn, IMAP_LIST);

  return result;
}
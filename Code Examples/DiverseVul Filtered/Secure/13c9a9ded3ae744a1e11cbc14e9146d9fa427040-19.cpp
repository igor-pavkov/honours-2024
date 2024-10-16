static CURLcode imap_perform(struct connectdata *conn, bool *connected,
                             bool *dophase_done)
{
  /* This is IMAP and no proxy */
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  struct IMAP *imap = data->req.protop;
  struct imap_conn *imapc = &conn->proto.imapc;
  bool selected = FALSE;

  DEBUGF(infof(conn->data, "DO phase starts\n"));

  if(conn->data->set.opt_no_body) {
    /* Requested no body means no transfer */
    imap->transfer = FTPTRANSFER_INFO;
  }

  *dophase_done = FALSE; /* not done yet */

  /* Determine if the requested mailbox (with the same UIDVALIDITY if set)
     has already been selected on this connection */
  if(imap->mailbox && imapc->mailbox &&
     !strcmp(imap->mailbox, imapc->mailbox) &&
     (!imap->uidvalidity || !imapc->mailbox_uidvalidity ||
      !strcmp(imap->uidvalidity, imapc->mailbox_uidvalidity)))
    selected = TRUE;

  /* Start the first command in the DO phase */
  if(conn->data->set.upload || data->set.mimepost.kind != MIMEKIND_NONE)
    /* APPEND can be executed directly */
    result = imap_perform_append(conn);
  else if(imap->custom && (selected || !imap->mailbox))
    /* Custom command using the same mailbox or no mailbox */
    result = imap_perform_list(conn);
  else if(!imap->custom && selected && imap->uid)
    /* FETCH from the same mailbox */
    result = imap_perform_fetch(conn);
  else if(!imap->custom && selected && imap->query)
    /* SEARCH the current mailbox */
    result = imap_perform_search(conn);
  else if(imap->mailbox && !selected &&
         (imap->custom || imap->uid || imap->query))
    /* SELECT the mailbox */
    result = imap_perform_select(conn);
  else
    /* LIST */
    result = imap_perform_list(conn);

  if(result)
    return result;

  /* Run the state-machine */
  result = imap_multi_statemach(conn, dophase_done);

  *connected = conn->bits.tcpconnect[FIRSTSOCKET];

  if(*dophase_done)
    DEBUGF(infof(conn->data, "DO phase is complete\n"));

  return result;
}
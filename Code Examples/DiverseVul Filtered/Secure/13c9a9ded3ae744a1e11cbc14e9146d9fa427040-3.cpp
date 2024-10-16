static CURLcode imap_done(struct connectdata *conn, CURLcode status,
                          bool premature)
{
  CURLcode result = CURLE_OK;
  struct Curl_easy *data = conn->data;
  struct IMAP *imap = data->req.protop;

  (void)premature;

  if(!imap)
    return CURLE_OK;

  if(status) {
    connclose(conn, "IMAP done with bad status"); /* marked for closure */
    result = status;         /* use the already set error code */
  }
  else if(!data->set.connect_only && !imap->custom &&
          (imap->uid || data->set.upload ||
          data->set.mimepost.kind != MIMEKIND_NONE)) {
    /* Handle responses after FETCH or APPEND transfer has finished */
    if(!data->set.upload && data->set.mimepost.kind == MIMEKIND_NONE)
      state(conn, IMAP_FETCH_FINAL);
    else {
      /* End the APPEND command first by sending an empty line */
      result = Curl_pp_sendf(&conn->proto.imapc.pp, "%s", "");
      if(!result)
        state(conn, IMAP_APPEND_FINAL);
    }

    /* Run the state-machine

       TODO: when the multi interface is used, this _really_ should be using
       the imap_multi_statemach function but we have no general support for
       non-blocking DONE operations!
    */
    if(!result)
      result = imap_block_statemach(conn);
  }

  /* Cleanup our per-request based variables */
  Curl_safefree(imap->mailbox);
  Curl_safefree(imap->uidvalidity);
  Curl_safefree(imap->uid);
  Curl_safefree(imap->section);
  Curl_safefree(imap->partial);
  Curl_safefree(imap->query);
  Curl_safefree(imap->custom);
  Curl_safefree(imap->custom_params);

  /* Clear the transfer mode for the next request */
  imap->transfer = FTPTRANSFER_BODY;

  return result;
}
static CURLcode imap_perform_fetch(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct IMAP *imap = conn->data->req.protop;

  /* Check we have a UID */
  if(!imap->uid) {
    failf(conn->data, "Cannot FETCH without a UID.");
    return CURLE_URL_MALFORMAT;
  }

  /* Send the FETCH command */
  if(imap->partial)
    result = imap_sendf(conn, "FETCH %s BODY[%s]<%s>",
                        imap->uid,
                        imap->section ? imap->section : "",
                        imap->partial);
  else
    result = imap_sendf(conn, "FETCH %s BODY[%s]",
                        imap->uid,
                        imap->section ? imap->section : "");

  if(!result)
    state(conn, IMAP_FETCH);

  return result;
}
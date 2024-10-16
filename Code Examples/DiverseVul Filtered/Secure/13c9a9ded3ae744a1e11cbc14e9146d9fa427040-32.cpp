static CURLcode imap_perform_search(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  struct IMAP *imap = conn->data->req.protop;

  /* Check we have a query string */
  if(!imap->query) {
    failf(conn->data, "Cannot SEARCH without a query string.");
    return CURLE_URL_MALFORMAT;
  }

  /* Send the SEARCH command */
  result = imap_sendf(conn, "SEARCH %s", imap->query);

  if(!result)
    state(conn, IMAP_SEARCH);

  return result;
}
static CURLcode imap_do(struct connectdata *conn, bool *done)
{
  CURLcode result = CURLE_OK;

  *done = FALSE; /* default to false */

  /* Parse the URL path */
  result = imap_parse_url_path(conn);
  if(result)
    return result;

  /* Parse the custom request */
  result = imap_parse_custom_request(conn);
  if(result)
    return result;

  result = imap_regular_transfer(conn, done);

  return result;
}
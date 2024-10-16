static CURLcode imap_perform_logout(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;

  /* Send the LOGOUT command */
  result = imap_sendf(conn, "LOGOUT");

  if(!result)
    state(conn, IMAP_LOGOUT);

  return result;
}
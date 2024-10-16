static CURLcode imap_perform_authenticate(struct connectdata *conn,
                                          const char *mech,
                                          const char *initresp)
{
  CURLcode result = CURLE_OK;

  if(initresp) {
    /* Send the AUTHENTICATE command with the initial response */
    result = imap_sendf(conn, "AUTHENTICATE %s %s", mech, initresp);
  }
  else {
    /* Send the AUTHENTICATE command */
    result = imap_sendf(conn, "AUTHENTICATE %s", mech);
  }

  return result;
}
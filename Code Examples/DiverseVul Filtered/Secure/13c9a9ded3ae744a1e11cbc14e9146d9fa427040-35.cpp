static CURLcode imap_perform_login(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  char *user;
  char *passwd;

  /* Check we have a username and password to authenticate with and end the
     connect phase if we don't */
  if(!conn->bits.user_passwd) {
    state(conn, IMAP_STOP);

    return result;
  }

  /* Make sure the username and password are in the correct atom format */
  user = imap_atom(conn->user, false);
  passwd = imap_atom(conn->passwd, false);

  /* Send the LOGIN command */
  result = imap_sendf(conn, "LOGIN %s %s", user ? user : "",
                      passwd ? passwd : "");

  free(user);
  free(passwd);

  if(!result)
    state(conn, IMAP_LOGIN);

  return result;
}
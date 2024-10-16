static CURLcode set_login(struct connectdata *conn)
{
  CURLcode result = CURLE_OK;
  const char *setuser = CURL_DEFAULT_USER;
  const char *setpasswd = CURL_DEFAULT_PASSWORD;

  /* If our protocol needs a password and we have none, use the defaults */
  if((conn->handler->flags & PROTOPT_NEEDSPWD) && !conn->bits.user_passwd)
    ;
  else {
    setuser = "";
    setpasswd = "";
  }
  /* Store the default user */
  if(!conn->user) {
    conn->user = strdup(setuser);
    if(!conn->user)
      return CURLE_OUT_OF_MEMORY;
  }

  /* Store the default password */
  if(!conn->passwd) {
    conn->passwd = strdup(setpasswd);
    if(!conn->passwd)
      result = CURLE_OUT_OF_MEMORY;
  }

  /* if there's a user without password, consider password blank */
  if(conn->user && !conn->passwd) {
    conn->passwd = strdup("");
    if(!conn->passwd)
      result = CURLE_OUT_OF_MEMORY;
  }

  return result;
}
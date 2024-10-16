static CURLcode override_login(struct Curl_easy *data,
                               struct connectdata *conn,
                               char **userp, char **passwdp, char **optionsp)
{
  bool user_changed = FALSE;
  bool passwd_changed = FALSE;
  CURLUcode uc;
  if(data->set.str[STRING_USERNAME]) {
    free(*userp);
    *userp = strdup(data->set.str[STRING_USERNAME]);
    if(!*userp)
      return CURLE_OUT_OF_MEMORY;
    conn->bits.user_passwd = TRUE; /* enable user+password */
    user_changed = TRUE;
  }

  if(data->set.str[STRING_PASSWORD]) {
    free(*passwdp);
    *passwdp = strdup(data->set.str[STRING_PASSWORD]);
    if(!*passwdp)
      return CURLE_OUT_OF_MEMORY;
    conn->bits.user_passwd = TRUE; /* enable user+password */
    passwd_changed = TRUE;
  }

  if(data->set.str[STRING_OPTIONS]) {
    free(*optionsp);
    *optionsp = strdup(data->set.str[STRING_OPTIONS]);
    if(!*optionsp)
      return CURLE_OUT_OF_MEMORY;
  }

  conn->bits.netrc = FALSE;
  if(data->set.use_netrc != CURL_NETRC_IGNORED) {
    char *nuser = NULL;
    char *npasswd = NULL;
    int ret;

    if(data->set.use_netrc == CURL_NETRC_OPTIONAL)
      nuser = *userp; /* to separate otherwise identical machines */

    ret = Curl_parsenetrc(conn->host.name,
                          &nuser, &npasswd,
                          data->set.str[STRING_NETRC_FILE]);
    if(ret > 0) {
      infof(data, "Couldn't find host %s in the "
            DOT_CHAR "netrc file; using defaults\n",
            conn->host.name);
    }
    else if(ret < 0) {
      return CURLE_OUT_OF_MEMORY;
    }
    else {
      /* set bits.netrc TRUE to remember that we got the name from a .netrc
         file, so that it is safe to use even if we followed a Location: to a
         different host or similar. */
      conn->bits.netrc = TRUE;
      conn->bits.user_passwd = TRUE; /* enable user+password */

      if(data->set.use_netrc == CURL_NETRC_OPTIONAL) {
        /* prefer credentials outside netrc */
        if(nuser && !*userp) {
          free(*userp);
          *userp = nuser;
          user_changed = TRUE;
        }
        if(npasswd && !*passwdp) {
          free(*passwdp);
          *passwdp = npasswd;
          passwd_changed = TRUE;
        }
      }
      else {
        /* prefer netrc credentials */
        if(nuser) {
          free(*userp);
          *userp = nuser;
          user_changed = TRUE;
        }
        if(npasswd) {
          free(*passwdp);
          *passwdp = npasswd;
          passwd_changed = TRUE;
        }
      }
    }
  }

  /* for updated strings, we update them in the URL */
  if(user_changed) {
    uc = curl_url_set(data->state.uh, CURLUPART_USER, *userp, 0);
    if(uc)
      return Curl_uc_to_curlcode(uc);
  }
  if(passwd_changed) {
    uc = curl_url_set(data->state.uh, CURLUPART_PASSWORD, *passwdp, 0);
    if(uc)
      return Curl_uc_to_curlcode(uc);
  }
  return CURLE_OK;
}
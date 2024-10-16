static void reuse_conn(struct connectdata *old_conn,
                       struct connectdata *conn)
{
  free(old_conn->proxy.rawalloc);

  /* free the SSL config struct from this connection struct as this was
     allocated in vain and is targeted for destruction */
  Curl_free_ssl_config(&old_conn->ssl_config);

  conn->data = old_conn->data;

  /* get the user+password information from the old_conn struct since it may
   * be new for this request even when we re-use an existing connection */
  conn->bits.user_passwd = old_conn->bits.user_passwd;
  if(conn->bits.user_passwd) {
    /* use the new user name and password though */
    Curl_safefree(conn->user);
    Curl_safefree(conn->passwd);
    conn->user = old_conn->user;
    conn->passwd = old_conn->passwd;
    old_conn->user = NULL;
    old_conn->passwd = NULL;
  }

  conn->bits.proxy_user_passwd = old_conn->bits.proxy_user_passwd;
  if(conn->bits.proxy_user_passwd) {
    /* use the new proxy user name and proxy password though */
    Curl_safefree(conn->proxyuser);
    Curl_safefree(conn->proxypasswd);
    conn->proxyuser = old_conn->proxyuser;
    conn->proxypasswd = old_conn->proxypasswd;
    old_conn->proxyuser = NULL;
    old_conn->proxypasswd = NULL;
  }

  /* host can change, when doing keepalive with a proxy or if the case is
     different this time etc */
  Curl_safefree(conn->host.rawalloc);
  conn->host=old_conn->host;

  /* persist connection info in session handle */
  Curl_persistconninfo(conn);

  /* re-use init */
  conn->bits.reuse = TRUE; /* yes, we're re-using here */

  Curl_safefree(old_conn->user);
  Curl_safefree(old_conn->passwd);
  Curl_safefree(old_conn->proxyuser);
  Curl_safefree(old_conn->proxypasswd);
  Curl_safefree(old_conn->localdev);

  Curl_llist_destroy(old_conn->send_pipe, NULL);
  Curl_llist_destroy(old_conn->recv_pipe, NULL);

  old_conn->send_pipe = NULL;
  old_conn->recv_pipe = NULL;

  Curl_safefree(old_conn->master_buffer);
}
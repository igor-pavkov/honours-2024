static void conn_free(struct connectdata *conn)
{
  if(!conn)
    return;

  /* possible left-overs from the async name resolvers */
  Curl_resolver_cancel(conn);

  /* close the SSL stuff before we close any sockets since they will/may
     write to the sockets */
  Curl_ssl_close(conn, FIRSTSOCKET);
  Curl_ssl_close(conn, SECONDARYSOCKET);

  /* close possibly still open sockets */
  if(CURL_SOCKET_BAD != conn->sock[SECONDARYSOCKET])
    Curl_closesocket(conn, conn->sock[SECONDARYSOCKET]);
  if(CURL_SOCKET_BAD != conn->sock[FIRSTSOCKET])
    Curl_closesocket(conn, conn->sock[FIRSTSOCKET]);
  if(CURL_SOCKET_BAD != conn->tempsock[0])
    Curl_closesocket(conn, conn->tempsock[0]);
  if(CURL_SOCKET_BAD != conn->tempsock[1])
    Curl_closesocket(conn, conn->tempsock[1]);

#if !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM) && \
    defined(NTLM_WB_ENABLED)
  Curl_ntlm_wb_cleanup(conn);
#endif

  Curl_safefree(conn->user);
  Curl_safefree(conn->passwd);
  Curl_safefree(conn->oauth_bearer);
  Curl_safefree(conn->options);
  Curl_safefree(conn->proxyuser);
  Curl_safefree(conn->proxypasswd);
  Curl_safefree(conn->allocptr.proxyuserpwd);
  Curl_safefree(conn->allocptr.uagent);
  Curl_safefree(conn->allocptr.userpwd);
  Curl_safefree(conn->allocptr.accept_encoding);
  Curl_safefree(conn->allocptr.te);
  Curl_safefree(conn->allocptr.rangeline);
  Curl_safefree(conn->allocptr.ref);
  Curl_safefree(conn->allocptr.host);
  Curl_safefree(conn->allocptr.cookiehost);
  Curl_safefree(conn->allocptr.rtsp_transport);
  Curl_safefree(conn->trailer);
  Curl_safefree(conn->host.rawalloc); /* host name buffer */
  Curl_safefree(conn->proxy.rawalloc); /* proxy name buffer */
  Curl_safefree(conn->master_buffer);

  Curl_llist_destroy(conn->send_pipe, NULL);
  Curl_llist_destroy(conn->recv_pipe, NULL);

  conn->send_pipe = NULL;
  conn->recv_pipe = NULL;

  Curl_safefree(conn->localdev);
  Curl_free_ssl_config(&conn->ssl_config);

  free(conn); /* free all the connection oriented data */
}
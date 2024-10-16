CURLcode Curl_disconnect(struct Curl_easy *data,
                         struct connectdata *conn, bool dead_connection)
{
  if(!conn)
    return CURLE_OK; /* this is closed and fine already */

  if(!data) {
    DEBUGF(infof(data, "DISCONNECT without easy handle, ignoring\n"));
    return CURLE_OK;
  }

  /*
   * If this connection isn't marked to force-close, leave it open if there
   * are other users of it
   */
  if(CONN_INUSE(conn) && !dead_connection) {
    DEBUGF(infof(data, "Curl_disconnect when inuse: %zu\n", CONN_INUSE(conn)));
    return CURLE_OK;
  }

  conn->data = data;
  if(conn->dns_entry != NULL) {
    Curl_resolv_unlock(data, conn->dns_entry);
    conn->dns_entry = NULL;
  }

  Curl_hostcache_prune(data); /* kill old DNS cache entries */

#if !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM)
  /* Cleanup NTLM connection-related data */
  Curl_http_ntlm_cleanup(conn);
#endif

  if(conn->handler->disconnect)
    /* This is set if protocol-specific cleanups should be made */
    conn->handler->disconnect(conn, dead_connection);

    /* unlink ourselves! */
  infof(data, "Closing connection %ld\n", conn->connection_id);
  Curl_conncache_remove_conn(conn, TRUE);

  free_fixed_hostname(&conn->host);
  free_fixed_hostname(&conn->conn_to_host);
  free_fixed_hostname(&conn->http_proxy.host);
  free_fixed_hostname(&conn->socks_proxy.host);

  DEBUGASSERT(conn->data == data);
  /* this assumes that the pointer is still there after the connection was
     detected from the cache */
  Curl_ssl_close(conn, FIRSTSOCKET);

  conn_free(conn);
  return CURLE_OK;
}
CURLcode Curl_http_connect(struct connectdata *conn, bool *done)
{
  CURLcode result;

  /* We default to persistent connections. We set this already in this connect
     function to make the re-use checks properly be able to check this bit. */
  connkeep(conn, "HTTP default");

  /* the CONNECT procedure might not have been completed */
  result = Curl_proxy_connect(conn);
  if(result)
    return result;

  if(conn->tunnel_state[FIRSTSOCKET] == TUNNEL_CONNECT)
    /* nothing else to do except wait right now - we're not done here. */
    return CURLE_OK;

  if(conn->given->flags & PROTOPT_SSL) {
    /* perform SSL initialization */
    result = https_connecting(conn, done);
    if(result)
      return result;
  }
  else
    *done = TRUE;

  return CURLE_OK;
}
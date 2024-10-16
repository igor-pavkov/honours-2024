static struct connectdata *allocate_conn(struct Curl_easy *data)
{
  struct connectdata *conn = calloc(1, sizeof(struct connectdata));
  if(!conn)
    return NULL;

#ifdef USE_SSL
  /* The SSL backend-specific data (ssl_backend_data) objects are allocated as
     a separate array to ensure suitable alignment.
     Note that these backend pointers can be swapped by vtls (eg ssl backend
     data becomes proxy backend data). */
  {
    size_t sslsize = Curl_ssl->sizeof_ssl_backend_data;
    char *ssl = calloc(4, sslsize);
    if(!ssl) {
      free(conn);
      return NULL;
    }
    conn->ssl_extra = ssl;
    conn->ssl[0].backend = (void *)ssl;
    conn->ssl[1].backend = (void *)(ssl + sslsize);
    conn->proxy_ssl[0].backend = (void *)(ssl + 2 * sslsize);
    conn->proxy_ssl[1].backend = (void *)(ssl + 3 * sslsize);
  }
#endif

  conn->handler = &Curl_handler_dummy;  /* Be sure we have a handler defined
                                           already from start to avoid NULL
                                           situations and checks */

  /* and we setup a few fields in case we end up actually using this struct */

  conn->sock[FIRSTSOCKET] = CURL_SOCKET_BAD;     /* no file descriptor */
  conn->sock[SECONDARYSOCKET] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->tempsock[0] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->tempsock[1] = CURL_SOCKET_BAD; /* no file descriptor */
  conn->connection_id = -1;    /* no ID */
  conn->port = -1; /* unknown at this point */
  conn->remote_port = -1; /* unknown at this point */
#if defined(USE_RECV_BEFORE_SEND_WORKAROUND) && defined(DEBUGBUILD)
  conn->postponed[0].bindsock = CURL_SOCKET_BAD; /* no file descriptor */
  conn->postponed[1].bindsock = CURL_SOCKET_BAD; /* no file descriptor */
#endif /* USE_RECV_BEFORE_SEND_WORKAROUND && DEBUGBUILD */

  /* Default protocol-independent behavior doesn't support persistent
     connections, so we set this to force-close. Protocols that support
     this need to set this to FALSE in their "curl_do" functions. */
  connclose(conn, "Default to force-close");

  /* Store creation time to help future close decision making */
  conn->created = Curl_now();

  /* Store current time to give a baseline to keepalive connection times. */
  conn->keepalive = Curl_now();

  /* Store off the configured connection upkeep time. */
  conn->upkeep_interval_ms = data->set.upkeep_interval_ms;

  conn->data = data; /* Setup the association between this connection
                        and the Curl_easy */

  conn->http_proxy.proxytype = data->set.proxytype;
  conn->socks_proxy.proxytype = CURLPROXY_SOCKS4;

#ifdef CURL_DISABLE_PROXY

  conn->bits.proxy = FALSE;
  conn->bits.httpproxy = FALSE;
  conn->bits.socksproxy = FALSE;
  conn->bits.proxy_user_passwd = FALSE;
  conn->bits.tunnel_proxy = FALSE;

#else /* CURL_DISABLE_PROXY */

  /* note that these two proxy bits are now just on what looks to be
     requested, they may be altered down the road */
  conn->bits.proxy = (data->set.str[STRING_PROXY] &&
                      *data->set.str[STRING_PROXY]) ? TRUE : FALSE;
  conn->bits.httpproxy = (conn->bits.proxy &&
                          (conn->http_proxy.proxytype == CURLPROXY_HTTP ||
                           conn->http_proxy.proxytype == CURLPROXY_HTTP_1_0 ||
                           conn->http_proxy.proxytype == CURLPROXY_HTTPS)) ?
                           TRUE : FALSE;
  conn->bits.socksproxy = (conn->bits.proxy &&
                           !conn->bits.httpproxy) ? TRUE : FALSE;

  if(data->set.str[STRING_PRE_PROXY] && *data->set.str[STRING_PRE_PROXY]) {
    conn->bits.proxy = TRUE;
    conn->bits.socksproxy = TRUE;
  }

  conn->bits.proxy_user_passwd =
    (data->set.str[STRING_PROXYUSERNAME]) ? TRUE : FALSE;
  conn->bits.tunnel_proxy = data->set.tunnel_thru_httpproxy;

#endif /* CURL_DISABLE_PROXY */

  conn->bits.user_passwd = (data->set.str[STRING_USERNAME]) ? TRUE : FALSE;
  conn->bits.ftp_use_epsv = data->set.ftp_use_epsv;
  conn->bits.ftp_use_eprt = data->set.ftp_use_eprt;

  conn->ssl_config.verifystatus = data->set.ssl.primary.verifystatus;
  conn->ssl_config.verifypeer = data->set.ssl.primary.verifypeer;
  conn->ssl_config.verifyhost = data->set.ssl.primary.verifyhost;
  conn->proxy_ssl_config.verifystatus =
    data->set.proxy_ssl.primary.verifystatus;
  conn->proxy_ssl_config.verifypeer = data->set.proxy_ssl.primary.verifypeer;
  conn->proxy_ssl_config.verifyhost = data->set.proxy_ssl.primary.verifyhost;

  conn->ip_version = data->set.ipver;

#if !defined(CURL_DISABLE_HTTP) && defined(USE_NTLM) && \
    defined(NTLM_WB_ENABLED)
  conn->ntlm_auth_hlpr_socket = CURL_SOCKET_BAD;
  conn->ntlm_auth_hlpr_pid = 0;
  conn->challenge_header = NULL;
  conn->response_header = NULL;
#endif

  if(Curl_pipeline_wanted(data->multi, CURLPIPE_HTTP1) &&
     !conn->master_buffer) {
    /* Allocate master_buffer to be used for HTTP/1 pipelining */
    conn->master_buffer = calloc(MASTERBUF_SIZE, sizeof(char));
    if(!conn->master_buffer)
      goto error;
  }

  /* Initialize the pipeline lists */
  Curl_llist_init(&conn->send_pipe, (curl_llist_dtor) llist_dtor);
  Curl_llist_init(&conn->recv_pipe, (curl_llist_dtor) llist_dtor);

#ifdef HAVE_GSSAPI
  conn->data_prot = PROT_CLEAR;
#endif

  /* Store the local bind parameters that will be used for this connection */
  if(data->set.str[STRING_DEVICE]) {
    conn->localdev = strdup(data->set.str[STRING_DEVICE]);
    if(!conn->localdev)
      goto error;
  }
  conn->localportrange = data->set.localportrange;
  conn->localport = data->set.localport;

  /* the close socket stuff needs to be copied to the connection struct as
     it may live on without (this specific) Curl_easy */
  conn->fclosesocket = data->set.fclosesocket;
  conn->closesocket_client = data->set.closesocket_client;

  return conn;
  error:

  Curl_llist_destroy(&conn->send_pipe, NULL);
  Curl_llist_destroy(&conn->recv_pipe, NULL);

  free(conn->master_buffer);
  free(conn->localdev);
#ifdef USE_SSL
  free(conn->ssl_extra);
#endif
  free(conn);
  return NULL;
}
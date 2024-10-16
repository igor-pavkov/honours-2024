static CURLcode choose_mech(struct connectdata *conn)
{
  int ret;
  struct Curl_easy *data = conn->data;
  void *tmp_allocation;
  const struct Curl_sec_client_mech *mech = &Curl_krb5_client_mech;

  tmp_allocation = realloc(conn->app_data, mech->size);
  if(tmp_allocation == NULL) {
    failf(data, "Failed realloc of size %zu", mech->size);
    mech = NULL;
    return CURLE_OUT_OF_MEMORY;
  }
  conn->app_data = tmp_allocation;

  if(mech->init) {
    ret = mech->init(conn->app_data);
    if(ret) {
      infof(data, "Failed initialization for %s. Skipping it.\n",
            mech->name);
      return CURLE_FAILED_INIT;
    }
  }

  infof(data, "Trying mechanism %s...\n", mech->name);
  ret = ftp_send_command(conn, "AUTH %s", mech->name);
  if(ret < 0)
    return CURLE_COULDNT_CONNECT;

  if(ret/100 != 3) {
    switch(ret) {
    case 504:
      infof(data, "Mechanism %s is not supported by the server (server "
            "returned ftp code: 504).\n", mech->name);
      break;
    case 534:
      infof(data, "Mechanism %s was rejected by the server (server returned "
            "ftp code: 534).\n", mech->name);
      break;
    default:
      if(ret/100 == 5) {
        infof(data, "server does not support the security extensions\n");
        return CURLE_USE_SSL_FAILED;
      }
      break;
    }
    return CURLE_LOGIN_DENIED;
  }

  /* Authenticate */
  ret = mech->auth(conn->app_data, conn);

  if(ret != AUTH_CONTINUE) {
    if(ret != AUTH_OK) {
      /* Mechanism has dumped the error to stderr, don't error here. */
      return -1;
    }
    DEBUGASSERT(ret == AUTH_OK);

    conn->mech = mech;
    conn->sec_complete = 1;
    conn->recv[FIRSTSOCKET] = sec_recv;
    conn->send[FIRSTSOCKET] = sec_send;
    conn->recv[SECONDARYSOCKET] = sec_recv;
    conn->send[SECONDARYSOCKET] = sec_send;
    conn->command_prot = PROT_SAFE;
    /* Set the requested protection level */
    /* BLOCKING */
    (void)sec_set_protection_level(conn);
  }

  return CURLE_OK;
}
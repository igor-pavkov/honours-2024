CURLcode Curl_connect(struct Curl_easy *data,
                      struct connectdata **in_connect,
                      bool *asyncp,
                      bool *protocol_done)
{
  CURLcode result;

  *asyncp = FALSE; /* assume synchronous resolves by default */

  /* init the single-transfer specific data */
  Curl_free_request_state(data);
  memset(&data->req, 0, sizeof(struct SingleRequest));
  data->req.maxdownload = -1;

  /* call the stuff that needs to be called */
  result = create_conn(data, in_connect, asyncp);

  if(!result) {
    if(CONN_INUSE(*in_connect))
      /* pipelining */
      *protocol_done = TRUE;
    else if(!*asyncp) {
      /* DNS resolution is done: that's either because this is a reused
         connection, in which case DNS was unnecessary, or because DNS
         really did finish already (synch resolver/fast async resolve) */
      result = Curl_setup_conn(*in_connect, protocol_done);
    }
  }

  if(result == CURLE_NO_CONNECTION_AVAILABLE) {
    *in_connect = NULL;
    return result;
  }
  else if(result && *in_connect) {
    /* We're not allowed to return failure with memory left allocated in the
       connectdata struct, free those here */
    Curl_disconnect(data, *in_connect, TRUE);
    *in_connect = NULL; /* return a NULL */
  }

  return result;
}
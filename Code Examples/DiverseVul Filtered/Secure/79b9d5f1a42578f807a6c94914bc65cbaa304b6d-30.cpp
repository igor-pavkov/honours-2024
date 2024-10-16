CURLcode Curl_http_setup_conn(struct connectdata *conn)
{
  /* allocate the HTTP-specific struct for the SessionHandle, only to survive
     during this request */
  DEBUGASSERT(conn->data->req.protop == NULL);

  conn->data->req.protop = calloc(1, sizeof(struct HTTP));
  if(!conn->data->req.protop)
    return CURLE_OUT_OF_MEMORY;

  return CURLE_OK;
}
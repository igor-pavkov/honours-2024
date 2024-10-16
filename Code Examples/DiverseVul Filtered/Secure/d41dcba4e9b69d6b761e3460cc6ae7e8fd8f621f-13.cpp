static bool IsPipeliningPossible(const struct SessionHandle *handle,
                                 const struct connectdata *conn)
{
  /* If a HTTP protocol and pipelining is enabled */
  if(conn->handler->protocol & PROTO_FAMILY_HTTP) {

    if(Curl_pipeline_wanted(handle->multi, CURLPIPE_HTTP1) &&
       (handle->set.httpversion != CURL_HTTP_VERSION_1_0) &&
       (handle->set.httpreq == HTTPREQ_GET ||
        handle->set.httpreq == HTTPREQ_HEAD))
      /* didn't ask for HTTP/1.0 and a GET or HEAD */
      return TRUE;

    if(Curl_pipeline_wanted(handle->multi, CURLPIPE_MULTIPLEX) &&
       (handle->set.httpversion >= CURL_HTTP_VERSION_2))
      /* allows HTTP/2 */
      return TRUE;
  }
  return FALSE;
}
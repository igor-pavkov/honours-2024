static bool use_http_1_1plus(const struct SessionHandle *data,
                             const struct connectdata *conn)
{
  return ((data->set.httpversion >= CURL_HTTP_VERSION_1_1) ||
         ((data->set.httpversion != CURL_HTTP_VERSION_1_0) &&
          ((conn->httpversion == 11) ||
           ((conn->httpversion != 10) &&
            (data->state.httpversion != 10))))) ? TRUE : FALSE;
}
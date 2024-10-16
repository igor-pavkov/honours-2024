CURLcode Curl_add_custom_headers(struct connectdata *conn,
                                 bool is_connect,
                                 Curl_send_buffer *req_buffer)
{
  char *ptr;
  struct curl_slist *h[2];
  struct curl_slist *headers;
  int numlists=1; /* by default */
  struct SessionHandle *data = conn->data;
  int i;

  enum proxy_use proxy;

  if(is_connect)
    proxy = HEADER_CONNECT;
  else
    proxy = conn->bits.httpproxy && !conn->bits.tunnel_proxy?
      HEADER_PROXY:HEADER_SERVER;

  switch(proxy) {
  case HEADER_SERVER:
    h[0] = data->set.headers;
    break;
  case HEADER_PROXY:
    h[0] = data->set.headers;
    if(data->set.sep_headers) {
      h[1] = data->set.proxyheaders;
      numlists++;
    }
    break;
  case HEADER_CONNECT:
    if(data->set.sep_headers)
      h[0] = data->set.proxyheaders;
    else
      h[0] = data->set.headers;
    break;
  }

  /* loop through one or two lists */
  for(i=0; i < numlists; i++) {
    headers = h[i];

    while(headers) {
      ptr = strchr(headers->data, ':');
      if(ptr) {
        /* we require a colon for this to be a true header */

        ptr++; /* pass the colon */
        while(*ptr && ISSPACE(*ptr))
          ptr++;

        if(*ptr) {
          /* only send this if the contents was non-blank */

          if(conn->allocptr.host &&
             /* a Host: header was sent already, don't pass on any custom Host:
                header as that will produce *two* in the same request! */
             checkprefix("Host:", headers->data))
            ;
          else if(data->set.httpreq == HTTPREQ_POST_FORM &&
                  /* this header (extended by formdata.c) is sent later */
                  checkprefix("Content-Type:", headers->data))
            ;
          else if(conn->bits.authneg &&
                  /* while doing auth neg, don't allow the custom length since
                     we will force length zero then */
                  checkprefix("Content-Length", headers->data))
            ;
          else if(conn->allocptr.te &&
                  /* when asking for Transfer-Encoding, don't pass on a custom
                     Connection: */
                  checkprefix("Connection", headers->data))
            ;
          else {
            CURLcode result = Curl_add_bufferf(req_buffer, "%s\r\n",
                                               headers->data);
            if(result)
              return result;
          }
        }
      }
      else {
        ptr = strchr(headers->data, ';');
        if(ptr) {

          ptr++; /* pass the semicolon */
          while(*ptr && ISSPACE(*ptr))
            ptr++;

          if(*ptr) {
            /* this may be used for something else in the future */
          }
          else {
            if(*(--ptr) == ';') {
              CURLcode result;

              /* send no-value custom header if terminated by semicolon */
              *ptr = ':';
              result = Curl_add_bufferf(req_buffer, "%s\r\n",
                                        headers->data);
              if(result)
                return result;
            }
          }
        }
      }
      headers = headers->next;
    }
  }
  return CURLE_OK;
}
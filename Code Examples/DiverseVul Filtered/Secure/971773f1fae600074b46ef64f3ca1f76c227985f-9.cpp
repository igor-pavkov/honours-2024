static int create_response_ietf_00(handler_ctx *hctx) {
    request_st * const r = hctx->gw.r;

    /* "Origin" header is preferred
     * ("Sec-WebSocket-Origin" is from older drafts of websocket spec) */
    const buffer *origin = http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Origin"));
    if (NULL == origin) {
        origin =
          http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Origin"));
    }
    if (NULL == origin) {
        DEBUG_LOG_ERR("%s", "Origin header is invalid");
        return -1;
    }
    if (!r->http_host || buffer_is_blank(r->http_host)) {
        DEBUG_LOG_ERR("%s", "Host header does not exist");
        return -1;
    }

    /* calc MD5 sum from keys */
    if (create_MD5_sum(r) < 0) {
        DEBUG_LOG_ERR("%s", "Sec-WebSocket-Key is invalid");
        return -1;
    }

    http_header_response_set(r, HTTP_HEADER_UPGRADE,
                             CONST_STR_LEN("Upgrade"),
                             CONST_STR_LEN("websocket"));
  #if 0 /*(added later in http_response_write_header())*/
    http_header_response_append(r, HTTP_HEADER_CONNECTION,
                                CONST_STR_LEN("Connection"),
                                CONST_STR_LEN("upgrade"));
  #endif
  #if 0 /*(Sec-WebSocket-Origin header is not required for hybi-00)*/
    /* Note: it is insecure to simply reflect back origin provided by client
     * (if admin did not configure restricted list of valid origins)
     * (see wstunnel_check_request()) */
    http_header_response_set(r, HTTP_HEADER_OTHER,
                             CONST_STR_LEN("Sec-WebSocket-Origin"),
                             BUF_PTR_LEN(origin));
  #endif

    buffer * const value =
      http_header_response_set_ptr(r, HTTP_HEADER_OTHER,
                                   CONST_STR_LEN("Sec-WebSocket-Location"));
    if (buffer_is_equal_string(&r->uri.scheme, CONST_STR_LEN("https")))
        buffer_copy_string_len(value, CONST_STR_LEN("wss://"));
    else
        buffer_copy_string_len(value, CONST_STR_LEN("ws://"));
    buffer_append_str2(value, BUF_PTR_LEN(r->http_host),
                              BUF_PTR_LEN(&r->uri.path));
    return 0;
}
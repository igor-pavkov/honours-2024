static int wstunnel_check_request(request_st * const r, handler_ctx * const hctx) {
    const buffer * const vers =
      http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Version"));
    const long hybivers = (NULL != vers)
      ? light_isdigit(*vers->ptr) ? strtol(vers->ptr, NULL, 10) : -1
      : 0;
    if (hybivers < 0 || hybivers > INT_MAX) {
        DEBUG_LOG_ERR("%s", "invalid Sec-WebSocket-Version");
        r->http_status = 400; /* Bad Request */
        return -1;
    }

    /*(redundant since HTTP/1.1 required in mod_wstunnel_check_extension())*/
    if (!r->http_host || buffer_is_blank(r->http_host)) {
        DEBUG_LOG_ERR("%s", "Host header does not exist");
        r->http_status = 400; /* Bad Request */
        return -1;
    }

    if (!wstunnel_is_allowed_origin(r, hctx)) {
        return -1;
    }

    return (int)hybivers;
}
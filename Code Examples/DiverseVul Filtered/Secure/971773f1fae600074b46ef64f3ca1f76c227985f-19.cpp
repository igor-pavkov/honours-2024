static int wstunnel_is_allowed_origin(request_st * const r, handler_ctx * const hctx) {
    /* If allowed origins is set (and not empty list), fail closed if no match.
     * Note that origin provided in request header has not been normalized, so
     * change in case or other non-normal forms might not match allowed list */
    const array * const allowed_origins = hctx->conf.origins;
    const buffer *origin = NULL;
    size_t olen;

    if (NULL == allowed_origins || 0 == allowed_origins->used) {
        DEBUG_LOG_INFO("%s", "allowed origins not specified");
        return 1;
    }

    /* "Origin" header is preferred
     * ("Sec-WebSocket-Origin" is from older drafts of websocket spec) */
    origin = http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Origin"));
    if (NULL == origin) {
        origin =
          http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Origin"));
    }
    olen = origin ? buffer_clen(origin) : 0;
    if (0 == olen) {
        DEBUG_LOG_ERR("%s", "Origin header is invalid");
        r->http_status = 400; /* Bad Request */
        return 0;
    }

    for (size_t i = 0; i < allowed_origins->used; ++i) {
        buffer *b = &((data_string *)allowed_origins->data[i])->value;
        size_t blen = buffer_clen(b);
        if ((olen > blen ? origin->ptr[olen-blen-1] == '.' : olen == blen)
            && 0 == memcmp(origin->ptr+olen-blen, b->ptr, blen)) {
            DEBUG_LOG_INFO("%s matches allowed origin: %s",origin->ptr,b->ptr);
            return 1;
        }
    }
    DEBUG_LOG_INFO("%s does not match any allowed origins", origin->ptr);
    r->http_status = 403; /* Forbidden */
    return 0;
}
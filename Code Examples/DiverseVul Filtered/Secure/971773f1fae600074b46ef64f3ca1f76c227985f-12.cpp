static int create_response_rfc_6455(handler_ctx *hctx) {
    request_st * const r = hctx->gw.r;
  if (r->http_version == HTTP_VERSION_1_1) {
    SHA_CTX sha;
    unsigned char sha_digest[SHA_DIGEST_LENGTH];

    const buffer *value_wskey =
      http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Key"));
    if (NULL == value_wskey) {
        DEBUG_LOG_ERR("%s", "Sec-WebSocket-Key is invalid");
        return -1;
    }

    /* get SHA1 hash of key */
    /* refer: RFC-6455 Sec.1.3 Opening Handshake */
    SHA1_Init(&sha);
    SHA1_Update(&sha, (const unsigned char *)BUF_PTR_LEN(value_wskey));
    SHA1_Update(&sha, (const unsigned char *)CONST_STR_LEN("258EAFA5-E914-47DA-95CA-C5AB0DC85B11"));
    SHA1_Final(sha_digest, &sha);

    http_header_response_set(r, HTTP_HEADER_UPGRADE,
                             CONST_STR_LEN("Upgrade"),
                             CONST_STR_LEN("websocket"));
  #if 0 /*(added later in http_response_write_header())*/
    http_header_response_append(r, HTTP_HEADER_CONNECTION,
                                CONST_STR_LEN("Connection"),
                                CONST_STR_LEN("upgrade"));
  #endif

    buffer * const value =
      http_header_response_set_ptr(r, HTTP_HEADER_OTHER,
                                   CONST_STR_LEN("Sec-WebSocket-Accept"));
    buffer_append_base64_encode(value, sha_digest, SHA_DIGEST_LENGTH, BASE64_STANDARD);
  }

    if (hctx->frame.type == MOD_WEBSOCKET_FRAME_TYPE_BIN)
        http_header_response_set(r, HTTP_HEADER_OTHER,
                                 CONST_STR_LEN("Sec-WebSocket-Protocol"),
                                 CONST_STR_LEN("binary"));
    else if (-1 == hctx->subproto)
        http_header_response_set(r, HTTP_HEADER_OTHER,
                                 CONST_STR_LEN("Sec-WebSocket-Protocol"),
                                 CONST_STR_LEN("base64"));

    return 0;
}
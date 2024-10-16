handler_t mod_wstunnel_handshake_create_response(handler_ctx *hctx) {
    request_st * const r = hctx->gw.r;
  #ifdef _MOD_WEBSOCKET_SPEC_RFC_6455_
    if (hctx->hybivers >= 8) {
        DEBUG_LOG_DEBUG("%s", "send handshake response");
        if (0 != create_response_rfc_6455(hctx)) {
            r->http_status = 400; /* Bad Request */
            return HANDLER_ERROR;
        }
        return HANDLER_GO_ON;
    }
  #endif /* _MOD_WEBSOCKET_SPEC_RFC_6455_ */

  #ifdef _MOD_WEBSOCKET_SPEC_IETF_00_
    if (hctx->hybivers == 0 && r->http_version == HTTP_VERSION_1_1) {
      #ifdef _MOD_WEBSOCKET_SPEC_IETF_00_
        /* 8 bytes should have been sent with request
         * for draft-ietf-hybi-thewebsocketprotocol-00 */
        chunkqueue *cq = &r->reqbody_queue;
        if (chunkqueue_length(cq) < 8)
            return HANDLER_WAIT_FOR_EVENT;
      #endif /* _MOD_WEBSOCKET_SPEC_IETF_00_ */

        DEBUG_LOG_DEBUG("%s", "send handshake response");
        if (0 != create_response_ietf_00(hctx)) {
            r->http_status = 400; /* Bad Request */
            return HANDLER_ERROR;
        }
        return HANDLER_GO_ON;
    }
  #endif /* _MOD_WEBSOCKET_SPEC_IETF_00_ */

    DEBUG_LOG_ERR("%s", "not supported WebSocket Version");
    r->http_status = 503; /* Service Unavailable */
    return HANDLER_ERROR;
}
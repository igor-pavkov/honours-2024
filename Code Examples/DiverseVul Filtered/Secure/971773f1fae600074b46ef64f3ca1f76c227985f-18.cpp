static handler_t wstunnel_create_env(gw_handler_ctx *gwhctx) {
    handler_ctx *hctx = (handler_ctx *)gwhctx;
    request_st * const r = hctx->gw.r;
    handler_t rc;
    if (0 == r->reqbody_length || r->http_version > HTTP_VERSION_1_1) {
        http_response_upgrade_read_body_unknown(r);
        chunkqueue_append_chunkqueue(&r->reqbody_queue, &r->read_queue);
    }
    rc = mod_wstunnel_handshake_create_response(hctx);
    if (rc != HANDLER_GO_ON) return rc;

    r->http_status = (r->http_version > HTTP_VERSION_1_1)
      ? 200  /* OK (response status for CONNECT) */
      : 101; /* Switching Protocols */
    r->resp_body_started = 1;

    hctx->ping_ts = log_monotonic_secs;
    gw_set_transparent(&hctx->gw);

    return HANDLER_GO_ON;
}
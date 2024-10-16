static handler_t wstunnel_stdin_append(gw_handler_ctx *gwhctx) {
    /* prepare websocket frames to backend */
    /* (caller should verify r->reqbody_queue) */
    /*assert(!chunkqueue_is_empty(&r->reqbody_queue));*/
    handler_ctx *hctx = (handler_ctx *)gwhctx;
    if (0 == mod_wstunnel_frame_recv(hctx))
        return HANDLER_GO_ON;
    else {
        /*(error)*/
        /* future: might differentiate client close request from client error,
         *         and then send 1000 or 1001 */
        request_st * const r = hctx->gw.r;
        DEBUG_LOG_INFO("disconnected from client (fd=%d)", r->con->fd);
        DEBUG_LOG_DEBUG("send close response to client (fd=%d)", r->con->fd);
        mod_wstunnel_frame_send(hctx, MOD_WEBSOCKET_FRAME_TYPE_CLOSE, CONST_STR_LEN("1000")); /* 1000 Normal Closure */
        gw_handle_request_reset(r, hctx->gw.plugin_data);
        return HANDLER_FINISHED;
    }
}
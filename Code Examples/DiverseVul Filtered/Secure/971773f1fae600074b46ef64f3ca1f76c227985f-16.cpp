static void wstunnel_backend_error(gw_handler_ctx *gwhctx) {
    handler_ctx *hctx = (handler_ctx *)gwhctx;
    if (hctx->gw.state == GW_STATE_WRITE || hctx->gw.state == GW_STATE_READ) {
        mod_wstunnel_frame_send(hctx, MOD_WEBSOCKET_FRAME_TYPE_CLOSE, CONST_STR_LEN("1001")); /* 1001 Going Away */
    }
}
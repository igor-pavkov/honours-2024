static void wstunnel_handler_ctx_free(void *gwhctx) {
    handler_ctx *hctx = (handler_ctx *)gwhctx;
    chunk_buffer_release(hctx->frame.payload);
}
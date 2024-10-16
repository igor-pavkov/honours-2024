static void unmask_payload(handler_ctx *hctx) {
    buffer * const b = hctx->frame.payload;
    for (size_t i = 0, used = buffer_clen(b); i < used; ++i) {
        b->ptr[i] ^= hctx->frame.ctl.mask[hctx->frame.ctl.mask_cnt];
        hctx->frame.ctl.mask_cnt = (hctx->frame.ctl.mask_cnt + 1) % 4;
    }
}
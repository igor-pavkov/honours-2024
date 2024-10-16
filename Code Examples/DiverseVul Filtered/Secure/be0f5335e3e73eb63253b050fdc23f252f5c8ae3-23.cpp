AP_CORE_DECLARE_NONSTD(apr_status_t) ap_old_write_filter(
    ap_filter_t *f, apr_bucket_brigade *bb)
{
    old_write_filter_ctx *ctx = f->ctx;

    AP_DEBUG_ASSERT(ctx);

    if (ctx->bb != NULL) {
        /* whatever is coming down the pipe (we don't care), we
         * can simply insert our buffered data at the front and
         * pass the whole bundle down the chain.
         */
        APR_BRIGADE_PREPEND(bb, ctx->bb);
    }

    return ap_pass_brigade(f->next, bb);
}
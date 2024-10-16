AP_DECLARE(int) ap_rflush(request_rec *r)
{
    conn_rec *c = r->connection;
    apr_bucket *b;
    ap_filter_t *f;
    old_write_filter_ctx *ctx;
    apr_status_t rv;

    f = insert_old_write_filter(r);
    ctx = f->ctx;

    b = apr_bucket_flush_create(c->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(ctx->tmpbb, b);

    rv = ap_pass_brigade(r->output_filters, ctx->tmpbb);
    apr_brigade_cleanup(ctx->tmpbb);
    if (rv != APR_SUCCESS)
        return -1;

    return 0;
}
static void error_output_stream(request_rec *r, int status)
{
    conn_rec *c = r->connection;
    apr_bucket_brigade *bb;
    apr_bucket *b;

    bb = apr_brigade_create(r->pool, c->bucket_alloc);
    b = ap_bucket_error_create(status, NULL, r->pool,
            r->connection->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(bb, b);
    b = apr_bucket_eos_create(c->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(bb, b);
    ap_pass_brigade(r->output_filters, bb);
}
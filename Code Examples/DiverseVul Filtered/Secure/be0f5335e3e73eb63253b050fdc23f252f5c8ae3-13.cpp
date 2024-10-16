AP_DECLARE(apr_size_t) ap_send_mmap(apr_mmap_t *mm,
                                    request_rec *r,
                                    apr_size_t offset,
                                    apr_size_t length)
{
    conn_rec *c = r->connection;
    apr_bucket_brigade *bb = NULL;
    apr_bucket *b;

    bb = apr_brigade_create(r->pool, c->bucket_alloc);
    b = apr_bucket_mmap_create(mm, offset, length, c->bucket_alloc);
    APR_BRIGADE_INSERT_TAIL(bb, b);
    ap_pass_brigade(r->output_filters, bb);

    return mm->size; /* XXX - change API to report apr_status_t? */
}
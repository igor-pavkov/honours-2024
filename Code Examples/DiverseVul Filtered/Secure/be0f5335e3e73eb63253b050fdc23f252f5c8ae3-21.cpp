AP_CORE_DECLARE_NONSTD(apr_status_t) ap_content_length_filter(
    ap_filter_t *f,
    apr_bucket_brigade *b)
{
    request_rec *r = f->r;
    struct content_length_ctx *ctx;
    apr_bucket *e;
    int eos = 0;
    apr_read_type_e eblock = APR_NONBLOCK_READ;

    ctx = f->ctx;
    if (!ctx) {
        f->ctx = ctx = apr_palloc(r->pool, sizeof(*ctx));
        ctx->data_sent = 0;
        ctx->tmpbb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
    }

    /* Loop through this set of buckets to compute their length
     */
    e = APR_BRIGADE_FIRST(b);
    while (e != APR_BRIGADE_SENTINEL(b)) {
        if (APR_BUCKET_IS_EOS(e)) {
            eos = 1;
            break;
        }
        if (e->length == (apr_size_t)-1) {
            apr_size_t len;
            const char *ignored;
            apr_status_t rv;

            /* This is probably a pipe bucket.  Send everything
             * prior to this, and then read the data for this bucket.
             */
            rv = apr_bucket_read(e, &ignored, &len, eblock);
            if (rv == APR_SUCCESS) {
                /* Attempt a nonblocking read next time through */
                eblock = APR_NONBLOCK_READ;
                r->bytes_sent += len;
            }
            else if (APR_STATUS_IS_EAGAIN(rv)) {
                /* Output everything prior to this bucket, and then
                 * do a blocking read on the next batch.
                 */
                if (e != APR_BRIGADE_FIRST(b)) {
                    apr_bucket *flush;
                    apr_brigade_split_ex(b, e, ctx->tmpbb);
                    flush = apr_bucket_flush_create(r->connection->bucket_alloc);

                    APR_BRIGADE_INSERT_TAIL(b, flush);
                    rv = ap_pass_brigade(f->next, b);
                    if (rv != APR_SUCCESS || f->c->aborted) {
                        return rv;
                    }
                    apr_brigade_cleanup(b);
                    APR_BRIGADE_CONCAT(b, ctx->tmpbb);
                    e = APR_BRIGADE_FIRST(b);

                    ctx->data_sent = 1;
                }
                eblock = APR_BLOCK_READ;
                continue;
            }
            else {
                ap_log_rerror(APLOG_MARK, APLOG_ERR, rv, r, APLOGNO(00574)
                              "ap_content_length_filter: "
                              "apr_bucket_read() failed");
                return rv;
            }
        }
        else {
            r->bytes_sent += e->length;
        }
        e = APR_BUCKET_NEXT(e);
    }

    /* If we've now seen the entire response and it's otherwise
     * okay to set the C-L in the response header, then do so now.
     *
     * We can only set a C-L in the response header if we haven't already
     * sent any buckets on to the next output filter for this request.
     */
    if (ctx->data_sent == 0 && eos &&
        /* don't whack the C-L if it has already been set for a HEAD
         * by something like proxy.  the brigade only has an EOS bucket
         * in this case, making r->bytes_sent zero.
         *
         * if r->bytes_sent > 0 we have a (temporary) body whose length may
         * have been changed by a filter.  the C-L header might not have been
         * updated so we do it here.  long term it would be cleaner to have
         * such filters update or remove the C-L header, and just use it
         * if present.
         */
        !(r->header_only && r->bytes_sent == 0 &&
            apr_table_get(r->headers_out, "Content-Length"))) {
        ap_set_content_length(r, r->bytes_sent);
    }

    ctx->data_sent = 1;
    return ap_pass_brigade(f->next, b);
}
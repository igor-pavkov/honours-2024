AP_DECLARE(apr_status_t) ap_send_fd(apr_file_t *fd, request_rec *r,
                                    apr_off_t offset, apr_size_t len,
                                    apr_size_t *nbytes)
{
    conn_rec *c = r->connection;
    apr_bucket_brigade *bb = NULL;
    apr_status_t rv;

    bb = apr_brigade_create(r->pool, c->bucket_alloc);

    apr_brigade_insert_file(bb, fd, offset, len, r->pool);

    rv = ap_pass_brigade(r->output_filters, bb);
    if (rv != APR_SUCCESS) {
        *nbytes = 0; /* no way to tell how many were actually sent */
    }
    else {
        *nbytes = len;
    }

    return rv;
}
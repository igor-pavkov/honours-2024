AP_DECLARE(int) ap_getline(char *s, int n, request_rec *r, int fold)
{
    char *tmp_s = s;
    apr_status_t rv;
    apr_size_t len;
    apr_bucket_brigade *tmp_bb;

    tmp_bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
    rv = ap_rgetline(&tmp_s, n, &len, r, fold, tmp_bb);
    apr_brigade_destroy(tmp_bb);

    /* Map the out-of-space condition to the old API. */
    if (rv == APR_ENOSPC) {
        return n;
    }

    /* Anything else is just bad. */
    if (rv != APR_SUCCESS) {
        return -1;
    }

    return (int)len;
}
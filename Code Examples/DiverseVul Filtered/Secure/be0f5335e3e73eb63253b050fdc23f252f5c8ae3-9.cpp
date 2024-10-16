AP_DECLARE(void) ap_get_mime_headers(request_rec *r)
{
    apr_bucket_brigade *tmp_bb;
    tmp_bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);
    ap_get_mime_headers_core(r, tmp_bb);
    apr_brigade_destroy(tmp_bb);
}
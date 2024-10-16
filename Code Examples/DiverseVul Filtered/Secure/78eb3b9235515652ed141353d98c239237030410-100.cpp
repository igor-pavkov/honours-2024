static apr_status_t lua_websocket_readbytes(conn_rec* c, char* buffer, 
        apr_off_t len) 
{
    apr_bucket_brigade *brigade = apr_brigade_create(c->pool, c->bucket_alloc);
    apr_status_t rv;
    rv = ap_get_brigade(c->input_filters, brigade, AP_MODE_READBYTES, 
            APR_BLOCK_READ, len);
    if (rv == APR_SUCCESS) {
        if (!APR_BRIGADE_EMPTY(brigade)) {
            apr_bucket* bucket = APR_BRIGADE_FIRST(brigade);
            const char* data = NULL;
            apr_size_t data_length = 0;
            rv = apr_bucket_read(bucket, &data, &data_length, APR_BLOCK_READ);
            if (rv == APR_SUCCESS) {
                memcpy(buffer, data, len);
            }
            apr_bucket_delete(bucket);
        }
    }
    apr_brigade_cleanup(brigade);
    return rv;
}
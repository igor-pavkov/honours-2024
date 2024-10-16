static int lua_websocket_peek(lua_State *L) 
{
    apr_status_t rv;
    apr_bucket_brigade *brigade;
    
    request_rec *r = ap_lua_check_request_rec(L, 1);
    
    brigade = apr_brigade_create(r->connection->pool, 
            r->connection->bucket_alloc);
    rv = ap_get_brigade(r->connection->input_filters, brigade, 
            AP_MODE_READBYTES, APR_NONBLOCK_READ, 1);
    if (rv == APR_SUCCESS) {
        lua_pushboolean(L, 1);
    }
    else {
        lua_pushboolean(L, 0);
    }
    apr_brigade_cleanup(brigade);
    return 1;
}
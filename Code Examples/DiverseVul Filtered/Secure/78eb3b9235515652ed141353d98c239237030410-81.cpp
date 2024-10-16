static int req_ssl_is_https_field(request_rec *r)
{
    return ap_lua_ssl_is_https(r->connection);
}
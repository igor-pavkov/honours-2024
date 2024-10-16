static int req_ssl_var_lookup(lua_State *L)
{
    request_rec *r = ap_lua_check_request_rec(L, 1);
    const char *s = luaL_checkstring(L, 2);
    const char *res = ap_lua_ssl_val(r->pool, r->server, r->connection, r, 
                                     (char *)s);
    lua_pushstring(L, res);
    return 1;
}
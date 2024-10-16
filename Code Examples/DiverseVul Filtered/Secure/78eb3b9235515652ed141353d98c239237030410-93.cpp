static int lua_ap_unescape(lua_State *L)
{
    const char     *escaped;
    char           *plain;
    size_t x,
           y;
    request_rec    *r;
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    escaped = lua_tolstring(L, 2, &x);
    plain = apr_pstrdup(r->pool, escaped);
    y = ap_unescape_urlencoded(plain);
    if (!y) {
        lua_pushstring(L, plain);
        return 1;
    }
    return 0;
}
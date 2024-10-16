static int lua_ap_escape(lua_State *L)
{
    const char     *plain;
    char           *escaped;
    size_t x;
    request_rec    *r;
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    plain = lua_tolstring(L, 2, &x);
    escaped = ap_escape_urlencoded(r->pool, plain);
    lua_pushstring(L, escaped);
    return 1;
}
static int req_escape_html(lua_State *L)
{
    request_rec *r = ap_lua_check_request_rec(L, 1);
    const char *s = luaL_checkstring(L, 2);
    lua_pushstring(L, ap_escape_html(r->pool, s));
    return 1;
}
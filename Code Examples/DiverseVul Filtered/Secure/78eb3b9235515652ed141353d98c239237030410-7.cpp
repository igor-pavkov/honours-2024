static int lua_get_cookie(lua_State *L) 
{
    const char *key, *cookie;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    key = luaL_checkstring(L, 2);
    cookie = NULL;
    ap_cookie_read(r, key, &cookie, 0);
    if (cookie != NULL) {
        lua_pushstring(L, cookie);
        return 1;
    }
    return 0;
}
static int lua_ap_escape_logitem(lua_State *L)
{
    char           *returnValue;
    request_rec    *r;
    const char     *str;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    str = lua_tostring(L, 2);
    returnValue = ap_escape_logitem(r->pool, str);
    lua_pushstring(L, returnValue);
    return 1;
}
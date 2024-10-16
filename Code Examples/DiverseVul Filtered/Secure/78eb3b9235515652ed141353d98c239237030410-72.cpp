static int lua_ap_os_escape_path(lua_State *L)
{
    char           *returnValue;
    request_rec    *r;
    const char     *path;
    int partial = 0;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    path = lua_tostring(L, 2);
    if (lua_isboolean(L, 3))
        partial = lua_toboolean(L, 3);
    returnValue = ap_os_escape_path(r->pool, path, partial);
    lua_pushstring(L, returnValue);
    return 1;
}
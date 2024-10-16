static int lua_ap_custom_response(lua_State *L)
{
    request_rec    *r;
    int status;
    const char     *string;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TNUMBER);
    status = lua_tointeger(L, 2);
    luaL_checktype(L, 3, LUA_TSTRING);
    string = lua_tostring(L, 3);
    ap_custom_response(r, status, string);
    return 0;
}
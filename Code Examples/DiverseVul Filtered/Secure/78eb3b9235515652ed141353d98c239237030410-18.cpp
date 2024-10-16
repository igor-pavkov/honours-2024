static int lua_ap_get_server_name_for_url(lua_State *L)
{
    const char     *servername;
    request_rec    *r;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    servername = ap_get_server_name_for_url(r);
    lua_pushstring(L, servername);
    return 1;
}
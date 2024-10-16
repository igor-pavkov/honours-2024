static int lua_ap_set_keepalive(lua_State *L)
{
    int returnValue;
    request_rec    *r;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    returnValue = ap_set_keepalive(r);
    lua_pushboolean(L, returnValue);
    return 1;
}
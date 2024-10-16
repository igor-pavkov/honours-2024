static int lua_ap_make_etag(lua_State *L)
{
    char           *returnValue;
    request_rec    *r;
    int force_weak;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TBOOLEAN);
    force_weak = luaL_optint(L, 2, 0);
    returnValue = ap_make_etag(r, force_weak);
    lua_pushstring(L, returnValue);
    return 1;
}
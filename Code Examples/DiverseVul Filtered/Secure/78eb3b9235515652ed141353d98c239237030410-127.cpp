static int lua_ap_rflush (lua_State *L) {

    int returnValue;
    request_rec *r;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    returnValue = ap_rflush(r);
    lua_pushboolean(L, (returnValue == 0));
    return 1;
}
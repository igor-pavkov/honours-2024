static int lua_ap_send_interim_response(lua_State *L)
{
    request_rec    *r;
    int send_headers = 0;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    if (lua_isboolean(L, 2))
        send_headers = lua_toboolean(L, 2);
    ap_send_interim_response(r, send_headers);
    return 0;
}
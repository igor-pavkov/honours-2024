static int req_puts(lua_State *L)
{
    request_rec *r = ap_lua_check_request_rec(L, 1);

    int argc = lua_gettop(L);
    int i;

    for (i = 2; i <= argc; i++) {
        ap_rputs(luaL_checkstring(L, i), r);
    }
    return 0;
}
static int lua_ap_clock(lua_State *L)
{
    apr_time_t now;
    now = apr_time_now();
    lua_pushnumber(L, (lua_Number) now);
    return 1;
}
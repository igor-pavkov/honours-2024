static int lua_ap_usleep(lua_State *L)
{
    apr_interval_time_t msec;
    luaL_checktype(L, 1, LUA_TNUMBER);
    msec = (apr_interval_time_t)lua_tonumber(L, 1);
    apr_sleep(msec);
    return 0;
}
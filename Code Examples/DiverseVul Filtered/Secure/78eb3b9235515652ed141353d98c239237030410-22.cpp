static int lua_apr_date_parse_rfc(lua_State *L)
{
    const char *input;
    apr_time_t result;

    luaL_checktype(L, 1, LUA_TSTRING);
    input = lua_tostring(L, 1);
    result = apr_date_parse_rfc(input);
    if (result == 0)
        return 0;
    lua_pushnumber(L, (lua_Number)(result / APR_USEC_PER_SEC));
    return 1;
}
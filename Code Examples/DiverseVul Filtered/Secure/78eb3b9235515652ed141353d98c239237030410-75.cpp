static int lua_ap_strcmp_match(lua_State *L)
{
    int returnValue;
    const char     *str;
    const char     *expected;
    int ignoreCase = 0;
    luaL_checktype(L, 1, LUA_TSTRING);
    str = lua_tostring(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    expected = lua_tostring(L, 2);
    if (lua_isboolean(L, 3))
        ignoreCase = lua_toboolean(L, 3);
    if (!ignoreCase)
        returnValue = ap_strcmp_match(str, expected);
    else
        returnValue = ap_strcasecmp_match(str, expected);
    lua_pushboolean(L, (!returnValue));
    return 1;
}
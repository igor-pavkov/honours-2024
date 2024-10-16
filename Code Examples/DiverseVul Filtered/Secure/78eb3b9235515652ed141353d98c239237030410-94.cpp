static int lua_ap_exists_config_define(lua_State *L)
{
    int returnValue;
    const char     *name;
    luaL_checktype(L, 1, LUA_TSTRING);
    name = lua_tostring(L, 1);
    returnValue = ap_exists_config_define(name);
    lua_pushboolean(L, returnValue);
    return 1;
}
static int lua_ap_state_query(lua_State *L)
{

    int returnValue;
    int query_code;
    luaL_checktype(L, 1, LUA_TNUMBER);
    query_code = lua_tointeger(L, 1);
    returnValue = ap_state_query(query_code);
    lua_pushinteger(L, returnValue);
    return 1;
}
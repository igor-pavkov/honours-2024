static int lua_ap_mpm_query(lua_State *L)
{
    int x,
        y;

    x = lua_tointeger(L, 1);
    ap_mpm_query(x, &y);
    lua_pushinteger(L, y);
    return 1;
}
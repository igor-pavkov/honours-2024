static int lua_ap_runtime_dir_relative(lua_State *L)
{
    request_rec    *r;
    const char     *file;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    file = luaL_optstring(L, 2, ".");
    lua_pushstring(L, ap_runtime_dir_relative(r->pool, file));
    return 1;
}
static int lua_apr_rmdir(lua_State *L)
{
    request_rec     *r;
    const char      *path;
    apr_status_t    status;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    path = lua_tostring(L, 2);
    status = apr_dir_remove(path, r->pool);
    lua_pushboolean(L, (status == 0));
    return 1;
}
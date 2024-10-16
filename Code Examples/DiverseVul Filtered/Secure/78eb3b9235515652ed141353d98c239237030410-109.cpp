static int lua_apr_mkrdir(lua_State *L)
{
    request_rec     *r;
    const char      *path;
    apr_status_t    status;
    apr_fileperms_t perms;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    path = lua_tostring(L, 2);
    perms = luaL_optinteger(L, 3, APR_OS_DEFAULT);
    status = apr_dir_make_recursive(path, perms, r->pool);
    lua_pushboolean(L, (status == 0));
    return 1;
}
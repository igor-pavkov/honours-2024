static int lua_apr_touch(lua_State *L)
{
    request_rec     *r;
    const char      *path;
    apr_status_t    status;
    apr_time_t      mtime;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    path = lua_tostring(L, 2);
    mtime = (apr_time_t)luaL_optnumber(L, 3, (lua_Number)apr_time_now());
    status = apr_file_mtime_set(path, mtime, r->pool);
    lua_pushboolean(L, (status == 0));
    return 1;
}
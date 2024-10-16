static int lua_ap_getdir(lua_State *L)
{
    request_rec    *r;
    apr_dir_t      *thedir;
    apr_finfo_t    file_info;
    apr_status_t   status;
    const char     *directory;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    directory = lua_tostring(L, 2);
    if (apr_dir_open(&thedir, directory, r->pool) == APR_SUCCESS) {
        int i = 0;
        lua_newtable(L);
        do {
            status = apr_dir_read(&file_info, APR_FINFO_NAME, thedir);
            if (APR_STATUS_IS_INCOMPLETE(status)) {
                continue; /* ignore un-stat()able files */
            }
            else if (status != APR_SUCCESS) {
                break;
            }
            lua_pushinteger(L, ++i);
            lua_pushstring(L, file_info.name);
            lua_settable(L, -3);

        } while (1);
        apr_dir_close(thedir);
        return 1;
    }
    else {
        return 0;
    }
}
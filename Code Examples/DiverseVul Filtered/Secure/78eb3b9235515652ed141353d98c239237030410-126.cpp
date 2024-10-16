static int lua_ap_sendfile(lua_State *L)
{

    apr_finfo_t file_info;
    const char  *filename;
    request_rec *r;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    filename = lua_tostring(L, 2);
    apr_stat(&file_info, filename, APR_FINFO_MIN, r->pool);
    if (file_info.filetype == APR_NOFILE || file_info.filetype == APR_DIR) {
        lua_pushboolean(L, 0);
    }
    else {
        apr_size_t      sent;
        apr_status_t    rc;
        apr_file_t      *file;

        rc = apr_file_open(&file, filename, APR_READ, APR_OS_DEFAULT,
                            r->pool);
        if (rc == APR_SUCCESS) {
            ap_send_fd(file, r, 0, (apr_size_t)file_info.size, &sent);
            apr_file_close(file);
            lua_pushinteger(L, sent);
        }
        else {
            lua_pushboolean(L, 0);
        }
    }

    return (1);
}
static int lua_ap_requestbody(lua_State *L)
{
    const char     *filename;
    request_rec    *r;
    apr_off_t      maxSize;
    
    r = ap_lua_check_request_rec(L, 1);
    filename = luaL_optstring(L, 2, 0);
    maxSize = luaL_optint(L, 3, 0);

    if (r) {
        apr_off_t size;
        if (maxSize > 0 && r->remaining > maxSize) {
            lua_pushnil(L);
            lua_pushliteral(L, "Request body was larger than the permitted size.");
            return 2;
        }
        if (r->method_number != M_POST && r->method_number != M_PUT)
            return (0);
        if (!filename) {
            const char     *data;

            if (lua_read_body(r, &data, &size, maxSize) != OK)
                return (0);

            lua_pushlstring(L, data, (size_t) size);
            lua_pushinteger(L, (lua_Integer) size);
            return (2);
        } else {
            apr_status_t rc;
            apr_file_t     *file;

            rc = apr_file_open(&file, filename, APR_CREATE | APR_FOPEN_WRITE,
                               APR_FPROT_OS_DEFAULT, r->pool);
            lua_settop(L, 0);
            if (rc == APR_SUCCESS) {
                rc = lua_write_body(r, file, &size);
                apr_file_close(file);
                if (rc != OK) {
                    lua_pushboolean(L, 0);
                    return 1;
                }
                lua_pushinteger(L, (lua_Integer) size);
                return (1);
            } else
                lua_pushboolean(L, 0);
            return (1);
        }
    }

    return (0);
}
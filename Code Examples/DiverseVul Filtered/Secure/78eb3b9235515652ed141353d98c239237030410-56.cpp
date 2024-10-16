static int lua_ap_stat(lua_State *L)
{
    request_rec    *r;
    const char     *filename;
    apr_finfo_t file_info;
    apr_int32_t wanted;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    filename = lua_tostring(L, 2);
    wanted = luaL_optinteger(L, 3, APR_FINFO_MIN);
    if (apr_stat(&file_info, filename, wanted, r->pool) == OK) {
        lua_newtable(L);
        if (wanted & APR_FINFO_MTIME) {
            lua_pushstring(L, "mtime");
            lua_pushnumber(L, (lua_Number) file_info.mtime);
            lua_settable(L, -3);
        }
        if (wanted & APR_FINFO_ATIME) {
            lua_pushstring(L, "atime");
            lua_pushnumber(L, (lua_Number) file_info.atime);
            lua_settable(L, -3);
        }
        if (wanted & APR_FINFO_CTIME) {
            lua_pushstring(L, "ctime");
            lua_pushnumber(L, (lua_Number) file_info.ctime);
            lua_settable(L, -3);
        }
        if (wanted & APR_FINFO_SIZE) {
            lua_pushstring(L, "size");
            lua_pushnumber(L, (lua_Number) file_info.size);
            lua_settable(L, -3);
        }
        if (wanted & APR_FINFO_TYPE) {
            lua_pushstring(L, "filetype");
            lua_pushinteger(L, file_info.filetype);
            lua_settable(L, -3);
        }
        if (wanted & APR_FINFO_PROT) {
            lua_pushstring(L, "protection");
            lua_pushinteger(L, file_info.protection);
            lua_settable(L, -3);
        }
        return 1;
    }
    else {
        return 0;
    }
}
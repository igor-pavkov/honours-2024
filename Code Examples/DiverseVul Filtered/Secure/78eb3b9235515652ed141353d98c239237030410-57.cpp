static int lua_apr_md5(lua_State *L)
{
    const char     *buffer;
    char           *result;
    size_t len;
    request_rec    *r;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    buffer = lua_tolstring(L, 2, &len);
    result = ap_md5_binary(r->pool, (const unsigned char *)buffer, len);
    lua_pushstring(L, result);
    return 1;
}
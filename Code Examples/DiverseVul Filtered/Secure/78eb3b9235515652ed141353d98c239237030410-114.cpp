static int lua_apr_sha1(lua_State *L)
{
    unsigned char digest[APR_SHA1_DIGESTSIZE];
    apr_sha1_ctx_t sha1;
    const char     *buffer;
    char           *result;
    size_t len;
    request_rec    *r;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    result = apr_pcalloc(r->pool, sizeof(digest) * 2 + 1);
    buffer = lua_tolstring(L, 2, &len);
    apr_sha1_init(&sha1);
    apr_sha1_update(&sha1, buffer, len);
    apr_sha1_final(digest, &sha1);
    
    ap_bin2hex(digest, sizeof(digest), result);
    lua_pushstring(L, result);
    return 1;
}
static int lua_apr_b64decode(lua_State *L)
{
    const char     *encoded;
    char           *plain;
    size_t          encoded_len, decoded_len;
    request_rec    *r;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    encoded = lua_tolstring(L, 2, &encoded_len);
    decoded_len = apr_base64_decode_len(encoded);
    if (decoded_len) {
        plain = apr_palloc(r->pool, decoded_len);
        decoded_len = apr_base64_decode(plain, encoded);
        if (decoded_len > 0 && plain[decoded_len - 1] == '\0')
            decoded_len--; 
        lua_pushlstring(L, plain, decoded_len);
        return 1;
    }
    return 0;
}
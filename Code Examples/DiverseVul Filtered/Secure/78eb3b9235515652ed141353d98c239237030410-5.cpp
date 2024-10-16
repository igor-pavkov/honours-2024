static int lua_apr_b64encode(lua_State *L)
{
    const char     *plain;
    char           *encoded;
    size_t          plain_len, encoded_len;
    request_rec    *r;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    plain = lua_tolstring(L, 2, &plain_len);
    encoded_len = apr_base64_encode_len(plain_len);
    if (encoded_len) {
        encoded = apr_palloc(r->pool, encoded_len);
        encoded_len = apr_base64_encode(encoded, plain, plain_len);
        if (encoded_len > 0 && encoded[encoded_len - 1] == '\0')
            encoded_len--; 
        lua_pushlstring(L, encoded, encoded_len);
        return 1;
    }
    return 0;
}
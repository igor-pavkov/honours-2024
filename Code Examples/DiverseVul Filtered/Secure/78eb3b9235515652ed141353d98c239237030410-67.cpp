static int req_newindex(lua_State *L)
{
    const char *key;
    /* request_rec* r = lua_touserdata(L, lua_upvalueindex(1)); */
    /* const char* key = luaL_checkstring(L, -2); */
    request_rec *r = ap_lua_check_request_rec(L, 1);
    key = luaL_checkstring(L, 2);

    if (0 == strcmp("args", key)) {
        const char *value = luaL_checkstring(L, 3);
        r->args = apr_pstrdup(r->pool, value);
        return 0;
    }

    if (0 == strcmp("content_type", key)) {
        const char *value = luaL_checkstring(L, 3);
        ap_set_content_type(r, apr_pstrdup(r->pool, value));
        return 0;
    }

    if (0 == strcmp("filename", key)) {
        const char *value = luaL_checkstring(L, 3);
        r->filename = apr_pstrdup(r->pool, value);
        return 0;
    }

    if (0 == strcmp("handler", key)) {
        const char *value = luaL_checkstring(L, 3);
        r->handler = apr_pstrdup(r->pool, value);
        return 0;
    }

    if (0 == strcmp("proxyreq", key)) {
        int value = luaL_checkinteger(L, 3);
        r->proxyreq = value;
        return 0;
    }

    if (0 == strcmp("status", key)) {
        int code = luaL_checkinteger(L, 3);
        r->status = code;
        return 0;
    }

    if (0 == strcmp("uri", key)) {
        const char *value = luaL_checkstring(L, 3);
        r->uri = apr_pstrdup(r->pool, value);
        return 0;
    }

    if (0 == strcmp("user", key)) {
        const char *value = luaL_checkstring(L, 3);
        r->user = apr_pstrdup(r->pool, value);
        return 0;
    }

    lua_pushstring(L,
                   apr_psprintf(r->pool,
                                "Property [%s] may not be set on a request_rec",
                                key));
    lua_error(L);
    return 0;
}
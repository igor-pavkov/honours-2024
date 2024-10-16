static int lua_ap_regex(lua_State *L)
{
    request_rec    *r;
    int i,
        rv,
        flags;
    const char     *pattern,
    *source;
    char           *err;
    ap_regex_t regex;
    ap_regmatch_t matches[MODLUA_MAX_REG_MATCH+1];

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    luaL_checktype(L, 3, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    source = lua_tostring(L, 2);
    pattern = lua_tostring(L, 3);
    flags = luaL_optinteger(L, 4, 0);

    rv = ap_regcomp(&regex, pattern, flags);
    if (rv) {
        lua_pushboolean(L, 0);
        err = apr_palloc(r->pool, 256);
        ap_regerror(rv, &regex, err, 256);
        lua_pushstring(L, err);
        return 2;
    }

    if (regex.re_nsub > MODLUA_MAX_REG_MATCH) {
        lua_pushboolean(L, 0);
        err = apr_palloc(r->pool, 64);
        apr_snprintf(err, 64,
                     "regcomp found %d matches; only %d allowed.",
                     regex.re_nsub, MODLUA_MAX_REG_MATCH);
        lua_pushstring(L, err);
        return 2;
    }

    rv = ap_regexec(&regex, source, MODLUA_MAX_REG_MATCH, matches, 0);
    if (rv == AP_REG_NOMATCH) {
        lua_pushboolean(L, 0);
        return 1;
    }
    
    lua_newtable(L);
    for (i = 0; i <= regex.re_nsub; i++) {
        lua_pushinteger(L, i);
        if (matches[i].rm_so >= 0 && matches[i].rm_eo >= 0)
            lua_pushstring(L,
                           apr_pstrndup(r->pool, source + matches[i].rm_so,
                                        matches[i].rm_eo - matches[i].rm_so));
        else
            lua_pushnil(L);
        lua_settable(L, -3);

    }
    return 1;
}
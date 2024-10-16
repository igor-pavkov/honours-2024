static int lua_ap_expr(lua_State *L)
{
    request_rec    *r;
    int x = 0;
    const char     *expr,
    *err;
    ap_expr_info_t res;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    expr = lua_tostring(L, 2);


    res.filename = NULL;
    res.flags = 0;
    res.line_number = 0;
    res.module_index = APLOG_MODULE_INDEX;

    err = ap_expr_parse(r->pool, r->pool, &res, expr, NULL);
    if (!err) {
        x = ap_expr_exec(r, &res, &err);
        lua_pushboolean(L, x);
        if (x < 0) {
            lua_pushstring(L, err);
            return 2;
        }
        return 1;
    } else {
        lua_pushboolean(L, 0);
        lua_pushstring(L, err);
        return 2;
    }
    lua_pushboolean(L, 0);
    return 1;
}
static int lua_apr_htpassword(lua_State *L)
{
    passwd_ctx     ctx = { 0 };
    request_rec    *r;

    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    ctx.passwd = apr_pstrdup(r->pool, lua_tostring(L, 2));
    ctx.alg = luaL_optinteger(L, 3, ALG_APMD5);
    ctx.cost = luaL_optinteger(L, 4, 0);
    ctx.pool = r->pool;
    ctx.out = apr_pcalloc(r->pool, MAX_PASSWD_LEN);
    ctx.out_len = MAX_PASSWD_LEN;
    if (mk_password_hash(&ctx)) {
        lua_pushboolean(L, 0);
        lua_pushstring(L, ctx.errstr);
        return 2;
    } else {
        lua_pushstring(L, ctx.out);
    }
    return 1;
}
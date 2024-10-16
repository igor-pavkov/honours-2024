static int req_construct_url(lua_State *L)
{
    request_rec *r = ap_lua_check_request_rec(L, 1);
    const char *name = luaL_checkstring(L, 2);
    lua_pushstring(L, ap_construct_url(r->pool, name, r));
    return 1;
}
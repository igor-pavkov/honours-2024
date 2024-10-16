static int req_write(lua_State *L)
{
    request_rec *r = ap_lua_check_request_rec(L, 1);
    size_t n;
    int rv;
    const char *buf = luaL_checklstring(L, 2, &n);

    rv = ap_rwrite((void *) buf, n, r);
    lua_pushinteger(L, rv);
    return 1;
}
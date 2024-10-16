static int req_log_at(lua_State *L, int level)
{
    const char *msg;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    lua_Debug dbg;

    lua_getstack(L, 1, &dbg);
    lua_getinfo(L, "Sl", &dbg);

    msg = luaL_checkstring(L, 2);
    ap_log_rerror(dbg.source, dbg.currentline, APLOG_MODULE_INDEX, level, 0,
                  r, "%s", msg);
    return 0;
}
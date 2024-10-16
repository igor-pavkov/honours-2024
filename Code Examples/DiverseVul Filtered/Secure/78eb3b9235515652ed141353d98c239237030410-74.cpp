static int req_add_output_filter(lua_State *L)
{
    request_rec *r = ap_lua_check_request_rec(L, 1);
    const char *name = luaL_checkstring(L, 2);
    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01485) "adding output filter %s",
                  name);
    ap_add_output_filter(name, L, r, r->connection);
    return 0;
}
static int req_crit(lua_State *L)
{
    return req_log_at(L, APLOG_CRIT);
}
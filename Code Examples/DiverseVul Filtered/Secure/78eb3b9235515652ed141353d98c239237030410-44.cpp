static int req_notice(lua_State *L)
{
    return req_log_at(L, APLOG_NOTICE);
}
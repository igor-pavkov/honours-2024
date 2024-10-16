static int req_debug(lua_State *L)
{
    return req_log_at(L, APLOG_DEBUG);
}
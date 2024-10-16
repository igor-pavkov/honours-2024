static int req_alert(lua_State *L)
{
    return req_log_at(L, APLOG_ALERT);
}
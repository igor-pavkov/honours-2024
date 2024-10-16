static int req_warn(lua_State *L)
{
    return req_log_at(L, APLOG_WARNING);
}
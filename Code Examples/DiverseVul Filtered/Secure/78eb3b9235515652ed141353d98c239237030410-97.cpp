static int req_err(lua_State *L)
{
    return req_log_at(L, APLOG_ERR);
}
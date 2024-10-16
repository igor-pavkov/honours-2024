static int lua_ap_add_input_filter(lua_State *L)
{
    request_rec    *r;
    const char     *filterName;
    ap_filter_rec_t *filter;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    filterName = lua_tostring(L, 2);
    filter = ap_get_input_filter_handle(filterName);
    if (filter) {
        ap_add_input_filter_handle(filter, NULL, r, r->connection);
        lua_pushboolean(L, 1);
    } else
        lua_pushboolean(L, 0);
    return 1;
}
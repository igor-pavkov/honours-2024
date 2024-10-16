static int lua_ap_set_context_info(lua_State *L)
{
    request_rec    *r;
    const char     *prefix;
    const char     *document_root;
    luaL_checktype(L, 1, LUA_TUSERDATA);
    r = ap_lua_check_request_rec(L, 1);
    luaL_checktype(L, 2, LUA_TSTRING);
    prefix = lua_tostring(L, 2);
    luaL_checktype(L, 3, LUA_TSTRING);
    document_root = lua_tostring(L, 3);
    ap_set_context_info(r, prefix, document_root);
    return 0;
}
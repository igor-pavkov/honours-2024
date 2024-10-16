static int lua_ap_set_document_root(lua_State *L)
{
    request_rec    *r;
    const char     *root;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TSTRING);
    r = ap_lua_check_request_rec(L, 1);
    root = lua_tostring(L, 2);
    ap_set_document_root(r, root);
    return 0;
}
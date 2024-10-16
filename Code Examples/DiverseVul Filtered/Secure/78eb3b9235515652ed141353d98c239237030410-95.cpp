void ap_lua_push_request(lua_State *L, request_rec *r)
{
    lua_boxpointer(L, r);
    luaL_getmetatable(L, "Apache2.Request");
    lua_setmetatable(L, -2);
}
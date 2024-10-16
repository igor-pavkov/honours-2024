void ap_lua_push_server(lua_State *L, server_rec *s)
{
    lua_boxpointer(L, s);
    luaL_getmetatable(L, "Apache2.Server");
    lua_setmetatable(L, -2);
    luaL_getmetatable(L, "Apache2.Server");

    lua_pushstring(L, s->server_hostname);
    lua_setfield(L, -2, "server_hostname");

    lua_pop(L, 1);
}
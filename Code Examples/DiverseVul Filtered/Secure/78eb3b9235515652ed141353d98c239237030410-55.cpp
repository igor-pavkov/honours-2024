void ap_lua_push_connection(lua_State *L, conn_rec *c)
{
    req_table_t* t;
    lua_boxpointer(L, c);
    luaL_getmetatable(L, "Apache2.Connection");
    lua_setmetatable(L, -2);
    luaL_getmetatable(L, "Apache2.Connection");

    t = apr_pcalloc(c->pool, sizeof(req_table_t));
    t->t = c->notes;
    t->r = NULL;
    t->n = "notes";
    ap_lua_push_apr_table(L, t);
    lua_setfield(L, -2, "notes");

    lua_pushstring(L, c->client_ip);
    lua_setfield(L, -2, "client_ip");

    lua_pop(L, 1);
}
static void read_cfg_tree(lua_State *L, request_rec *r, ap_directive_t *rcfg) {
    int x = 0;
    const char* value;
    ap_directive_t *cfg;
    lua_newtable(L);
    
    for (cfg = rcfg; cfg; cfg = cfg->next) {
        x++;
        lua_pushnumber(L, x);
        lua_newtable(L);
        value = apr_psprintf(r->pool, "%s %s", cfg->directive, cfg->args);
        lua_pushstring(L, "directive");
        lua_pushstring(L, value);
        lua_settable(L, -3);
        lua_pushstring(L, "file");
        lua_pushstring(L, cfg->filename);
        lua_settable(L, -3);
        lua_pushstring(L, "line");
        lua_pushnumber(L, cfg->line_num);
        lua_settable(L, -3);
        if (cfg->first_child) {
            lua_pushstring(L, "children");
            read_cfg_tree(L, r, cfg->first_child);
            lua_settable(L, -3);
        }
        lua_settable(L, -3);
    }
}
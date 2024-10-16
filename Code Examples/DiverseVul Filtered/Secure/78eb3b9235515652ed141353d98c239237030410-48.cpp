static int lua_ap_module_info(lua_State *L)
{
    const char     *moduleName;
    module         *mod;

    luaL_checktype(L, 1, LUA_TSTRING);
    moduleName = lua_tostring(L, 1);
    mod = ap_find_linked_module(moduleName);
    if (mod && mod->cmds) {
        const command_rec *cmd;
        lua_newtable(L);
        lua_pushstring(L, "commands");
        lua_newtable(L);
        for (cmd = mod->cmds; cmd->name; ++cmd) {
            lua_pushstring(L, cmd->name);
            lua_pushstring(L, cmd->errmsg);
            lua_settable(L, -3);
        }
        lua_settable(L, -3);
        return 1;
    }
    return 0;
}
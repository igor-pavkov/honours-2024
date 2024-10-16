static int lua_ap_scoreboard_process(lua_State *L)
{
    int i;
    process_score  *ps_record;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TNUMBER);
    i = lua_tointeger(L, 2);
    ps_record = ap_get_scoreboard_process(i);
    if (ps_record) {
        lua_newtable(L);

        lua_pushstring(L, "connections");
        lua_pushnumber(L, ps_record->connections);
        lua_settable(L, -3);

        lua_pushstring(L, "keepalive");
        lua_pushnumber(L, ps_record->keep_alive);
        lua_settable(L, -3);

        lua_pushstring(L, "lingering_close");
        lua_pushnumber(L, ps_record->lingering_close);
        lua_settable(L, -3);

        lua_pushstring(L, "pid");
        lua_pushnumber(L, ps_record->pid);
        lua_settable(L, -3);

        lua_pushstring(L, "suspended");
        lua_pushnumber(L, ps_record->suspended);
        lua_settable(L, -3);

        lua_pushstring(L, "write_completion");
        lua_pushnumber(L, ps_record->write_completion);
        lua_settable(L, -3);

        lua_pushstring(L, "not_accepting");
        lua_pushnumber(L, ps_record->not_accepting);
        lua_settable(L, -3);

        lua_pushstring(L, "quiescing");
        lua_pushnumber(L, ps_record->quiescing);
        lua_settable(L, -3);

        return 1;
    }
    return 0;
}
static int lua_ap_server_info(lua_State *L)
{
    lua_newtable(L);

    lua_pushstring(L, "server_executable");
    lua_pushstring(L, ap_server_argv0);
    lua_settable(L, -3);

    lua_pushstring(L, "server_root");
    lua_pushstring(L, ap_server_root);
    lua_settable(L, -3);

    lua_pushstring(L, "scoreboard_fname");
    lua_pushstring(L, ap_scoreboard_fname);
    lua_settable(L, -3);

    lua_pushstring(L, "server_mpm");
    lua_pushstring(L, ap_show_mpm());
    lua_settable(L, -3);

    return 1;
}
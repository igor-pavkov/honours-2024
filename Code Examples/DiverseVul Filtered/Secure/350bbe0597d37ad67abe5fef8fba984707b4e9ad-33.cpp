int luaopen_libssh2 (lua_State *L) {
    lua_settop(L, 0); /* clear the stack */

    luaL_newlibtable(L, libssh2);

    lua_newtable(L); /* ssh2 session metatable */
    lua_pushvalue(L, -1);
    lua_pushcclosure(L, gc, 1);
    lua_setfield(L, -2, "__gc");
    lua_pushvalue(L, -1);
    lua_pushcclosure(L, filter, 1);
    lua_setfield(L, -2, "filter");

    luaL_setfuncs(L, libssh2, 1);

    static bool libssh2_initialized = false;
    if (!libssh2_initialized && (libssh2_init(0) != 0))
        luaL_error(L, "unable to open libssh2");
    libssh2_initialized = true;

    return 1;
}
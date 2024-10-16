static int setup_channel(lua_State *L, int status, lua_KContext ctx) {
    int rc;
    // ssh_userdata *state = (ssh_userdata *)lua_touserdata(L, 1);
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **) lua_touserdata(L, 2);

    while ((rc = libssh2_channel_request_pty(*channel, "vanilla")) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);
        lua_callk(L, 1, 0, 0, setup_channel);
    }
    if (rc != 0)
        return luaL_error(L, "Requesting pty");

    return 1;
}
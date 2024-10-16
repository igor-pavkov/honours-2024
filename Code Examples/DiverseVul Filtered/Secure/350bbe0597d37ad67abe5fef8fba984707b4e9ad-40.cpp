static int finish_open_channel (lua_State *L, int status, lua_KContext ctx) {
    ssh_userdata *state = (ssh_userdata *)lua_touserdata(L, 1);
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **) lua_touserdata(L, 2);

    while ((*channel = libssh2_channel_open_session(state->session)) == NULL
    && libssh2_session_last_errno(state->session) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);
        lua_callk(L, 1, 0, 0, finish_open_channel);
    }
    if (channel == NULL)
        return luaL_error(L, "Opening channel");

    return setup_channel(L, 0, 0);
}
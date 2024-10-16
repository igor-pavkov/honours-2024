static int l_open_channel (lua_State *L) {
    ssh_userdata *state = (ssh_userdata *)lua_touserdata(L, 1);
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **)lua_newuserdata(L, sizeof(LIBSSH2_CHANNEL *));

    while ((*channel = libssh2_channel_open_session(state->session)) == NULL
    && libssh2_session_last_errno(state->session) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);
        lua_callk(L, 1, 0, 0, finish_open_channel);
    }

    return l_setup_channel(L);
}
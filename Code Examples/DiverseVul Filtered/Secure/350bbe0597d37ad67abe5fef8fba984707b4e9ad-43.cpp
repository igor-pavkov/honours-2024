static int session_close (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    struct ssh_userdata *state;

    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");

    if (state->session != NULL) {
        while ((rc = libssh2_session_disconnect(
            state->session, "Normal Shutdown")) == LIBSSH2_ERROR_EAGAIN) {
            luaL_getmetafield(L, 1, "filter");
            lua_pushvalue(L, 1);

            assert(lua_status(L) == LUA_OK);
            lua_callk(L, 1, 0, 0, session_close);
        }

        if (rc < 0)
            return luaL_error(L, "unable to disconnect session");

        if (libssh2_session_free(state->session) < 0)
            return luaL_error(L, "unable to free session");

        state->session = NULL;
    }

    return 0;
}
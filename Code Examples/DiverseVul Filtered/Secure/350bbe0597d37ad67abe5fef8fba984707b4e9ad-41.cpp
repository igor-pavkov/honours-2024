static int finish_session_open (lua_State *L, int status, lua_KContext ctx) {
    assert(lua_gettop(L) == 6);
    if (lua_toboolean(L, -2)) {
        lua_pop(L, 2);
        return do_session_handshake(L,0,0);
    }
    else {
        struct ssh_userdata *state = NULL;

        state = (struct ssh_userdata *) nseU_checkudata(L, 3, SSH2_UDATA, "ssh2");
        if (state->session != NULL) {
            libssh2_session_free(state->session);
            state->session = NULL;
        }
        return lua_error(L);
    }
}
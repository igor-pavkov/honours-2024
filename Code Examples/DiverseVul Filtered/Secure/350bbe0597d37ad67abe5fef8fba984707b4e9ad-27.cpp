static int l_set_timeout(lua_State *L) {
    long timeout = luaL_checkinteger(L, 2);
    struct ssh_userdata *state = NULL;
    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");

    libssh2_session_set_timeout(state->session, timeout);

    return 0;
}
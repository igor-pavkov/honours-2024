static int userauth_password (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    const char *username, *password;
    struct ssh_userdata *state;

    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");
    username = luaL_checkstring(L, 2);
    password = luaL_checkstring(L, 3);

    assert(state->session != NULL);
    while ((rc = libssh2_userauth_password(state->session,
        username, password)) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);

        assert(lua_status(L) == LUA_OK);
        lua_callk(L, 1, 0, 0, userauth_password);
    }

    if (rc == 0)
        lua_pushboolean(L, 1);
    else
        lua_pushboolean(L, 0);

    return 1;
}
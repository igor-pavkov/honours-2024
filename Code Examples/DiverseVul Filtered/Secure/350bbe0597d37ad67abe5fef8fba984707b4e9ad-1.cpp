static int userauth_list (lua_State *L, int status, lua_KContext ctx) {
    char *auth_list = NULL;
    struct ssh_userdata *state = NULL;
    const char *username = luaL_checkstring(L, 2);

    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");
    assert(state->session != NULL);

    while ((auth_list = libssh2_userauth_list(state->session, username, lua_rawlen(L, 2))) == NULL
        && libssh2_session_last_errno(state->session) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);

        assert(lua_status(L) == LUA_OK);
        lua_callk(L, 1, 0, 0, userauth_list);
    }

    if (auth_list) {
        const char *auth = strtok(auth_list, ",");
        lua_newtable(L);
        do {
            lua_pushstring(L, auth);
            lua_rawseti(L, -2, lua_rawlen(L, -2) + 1);
        }
        while ((auth = strtok(NULL, ",")));

        //libssh2_free(state->session, (void *)auth_list);
    }
    else if (libssh2_userauth_authenticated(state->session)) {
        lua_pushliteral(L, "none_auth");
    }
    else {
        return ssh_error(L, state->session, "userauth_list");
    }

    return 1;
}
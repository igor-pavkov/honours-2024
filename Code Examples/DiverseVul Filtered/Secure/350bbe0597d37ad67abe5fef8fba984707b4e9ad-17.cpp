static int publickey_canauth (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    int errlen;
    char *errmsg;
    const char *username;
    unsigned const char *publickey_data;
    size_t len = 0;
    struct ssh_userdata *state;

    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");
    username = luaL_checkstring(L, 2);

    if (lua_isstring(L, 3))
        publickey_data = (unsigned const char*)lua_tolstring(L, 3, &len);
    else
        return luaL_error(L, "Invalid public key");

    while ((rc = libssh2_userauth_publickey(state->session,
        username, publickey_data, len, &publickey_canauth_cb, NULL)) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);

        assert(lua_status(L) == LUA_OK);
        lua_callk(L, 1, 0, 0, publickey_canauth);
    }

    libssh2_session_last_error(state->session, &errmsg, &errlen, 0);

    if (rc == LIBSSH2_ERROR_ALLOC || rc == LIBSSH2_ERROR_PUBLICKEY_UNVERIFIED)
        lua_pushboolean(L, 1); //Username/PublicKey combination invalid
    else if (rc == LIBSSH2_ERROR_AUTHENTICATION_FAILED)
        lua_pushboolean(L, 0);
    else
        return luaL_error(L, "Invalid Publickey");

    return 1;
}
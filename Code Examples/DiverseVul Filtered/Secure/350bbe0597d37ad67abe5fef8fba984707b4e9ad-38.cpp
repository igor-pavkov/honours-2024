static int userauth_publickey (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    const char *username, *private_key_file, *passphrase, *public_key_file;
    struct ssh_userdata *state = NULL;
    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");

    username = luaL_checkstring(L, 2);
    private_key_file = luaL_checkstring(L, 3);

    if (lua_isstring(L, 4))
        passphrase = lua_tostring(L, 4);
    else
        passphrase = NULL;

    if (lua_isstring(L, 5))
        public_key_file = lua_tostring(L, 5);
    else
        public_key_file = NULL;

    while ((rc = libssh2_userauth_publickey_fromfile(
        state->session, username, public_key_file, private_key_file, passphrase
        )) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);

        assert(lua_status(L) == LUA_OK);
        lua_callk(L, 1, 0, 0, userauth_publickey);
    }

    if (rc == 0)
        lua_pushboolean(L, 1);
    else
        lua_pushboolean(L, 0);

    return 1;
}
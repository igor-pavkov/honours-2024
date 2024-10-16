static int channel_send_eof(lua_State *L, int status, lua_KContext ctx) {
    int rc;
    // ssh_userdata *state = (ssh_userdata *)lua_touserdata(L, 1);
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **) lua_touserdata(L, 2);

    while ((rc = libssh2_channel_send_eof(*channel)) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);
        lua_callk(L, 1, 0, 0, channel_send_eof);
    }
    if (rc != 0)
        return luaL_error(L, "Error sending EOF");

    return 0;
}
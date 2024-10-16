static int l_channel_read_stderr(lua_State *L) {
    int rc;
    char buf[2048];
    size_t buflen = 2048;
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **) lua_touserdata(L, 2);

    while ((rc = libssh2_channel_read_stderr(*channel, buf, buflen)) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);
        lua_callk(L, 1, 0, 0, channel_read);
    }

    if (rc > 0) {
        lua_pushlstring(L, buf, rc);
        return 1;
    }
    else if (rc < 0)
        return luaL_error(L, "Reading from channel");

    lua_pushnil(L);
    return 1;
}
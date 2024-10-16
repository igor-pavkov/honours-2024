static int channel_write (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    const char *buf;
    size_t buflen = 0;
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **) lua_touserdata(L, 2);

    if (lua_isstring(L, 3))
        buf = lua_tolstring(L, 3, &buflen);
    else
        return luaL_error(L, "Invalid buffer");

    while ((rc = libssh2_channel_write(*channel, buf, buflen)) == LIBSSH2_ERROR_EAGAIN) {
        luaL_getmetafield(L, 1, "filter");
        lua_pushvalue(L, 1);
        lua_callk(L, 1, 0, 0, channel_write);
    }

    if (rc < 0)
        return luaL_error(L, "Writing to channel");

    lua_pushinteger(L, rc);
    return 1;
}
static int l_channel_eof(lua_State *L) {
    int result;
    LIBSSH2_CHANNEL **channel = (LIBSSH2_CHANNEL **) lua_touserdata(L, 1);

    result = libssh2_channel_eof(*channel);
    if (result >= 0)
        lua_pushboolean(L, result);
    else
        return luaL_error(L, "Error checking for EOF");

    return 1;
}
static int l_channel_send_eof(lua_State *L) {
    return channel_send_eof(L, 0, 0);
}
static int l_session_close (lua_State *L) {
    return session_close(L, 0, 0);
}
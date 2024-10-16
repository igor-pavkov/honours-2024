static int l_session_open (lua_State *L) {
    int rc;
    ssh_userdata *state = NULL;

    luaL_checkinteger(L, 2);
    lua_settop(L, 2);

    state = (ssh_userdata *)lua_newuserdata(L, sizeof(ssh_userdata)); /* index 3 */

    assert(lua_gettop(L) == 3);
    state->session = NULL;
    state->sp[0] = -1;
    state->sp[1] = -1;
    lua_pushvalue(L, lua_upvalueindex(1)); /* metatable */
    lua_setmetatable(L, 3);

    lua_newtable(L);
    lua_setuservalue(L, 3);
    lua_getuservalue(L, 3); /* index 4 - a table associated with userdata*/
    assert(lua_gettop(L) == 4);

    state->session = libssh2_session_init();

    if (state->session == NULL) {
        // A session could not be created because of memory limit
        return nseU_safeerror(L, "trying to initiate session");
    }

    libssh2_session_set_blocking(state->session, 0);

    if (make_socketpair(state->sp, 1) == -1)
        return nseU_safeerror(L, "trying to create socketpair");

#ifdef WIN32
    unsigned long s_mode = 1; // non-blocking

    rc = ioctlsocket(state->sp[1], FIONBIO, (unsigned long *)&s_mode);
    if (rc != NO_ERROR)
        return nseU_safeerror(L, "%s", strerror(errno));
#else
    // get file descriptor flags
    rc = fcntl(state->sp[1], F_GETFD);
    if (rc == -1)
        return nseU_safeerror(L, "%s", strerror(errno));

    // add non-blocking flag and update file descriptor flags
    rc |= O_NONBLOCK;
    rc = fcntl(state->sp[1], F_SETFL, rc);
    if (rc == -1)
        return nseU_safeerror(L, "%s", strerror(errno));
#endif

    lua_getglobal(L, "nmap");
    lua_getfield(L, -1, "new_socket");
    lua_replace(L, -2);
    lua_call(L, 0, 1);
    lua_setfield(L, 4, "sock");
    lua_pushliteral(L, "");
    lua_setfield(L, 4, "sp_buff");
    assert(lua_gettop(L) == 4);

    lua_getfield(L, 4, "sock");
    lua_getfield(L, -1, "connect");
    lua_insert(L, -2); /* swap */
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_callk(L, 3, 2, 3, finish_session_open);
    return finish_session_open(L,0,0);
}
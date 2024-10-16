static int filter (lua_State *L) {
    int rc;
    char data[4096];
    struct ssh_userdata *sshu = NULL;

    sshu = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");

    lua_getuservalue(L, 1);
    lua_getfield(L, -1, "sock");
    lua_replace(L, -2);

#ifdef WIN32
    rc = recv(sshu->sp[1], data, sizeof(data), 0);

    if (WSAGetLastError() == WSAEWOULDBLOCK)
        rc = 0;
#else
    rc = read(sshu->sp[1], data, sizeof(data));
#endif

    if (rc > 0) {
        //write data to nsock socket
        lua_getfield(L, -1, "send");
        lua_insert(L, -2); /* swap */
        lua_pushlstring(L, data, rc);

        assert(lua_status(L) == LUA_OK);
        lua_callk(L, 2, 2, 0, finish_send);

        return finish_send(L,0,0);
    }
    else if (rc == -1 && errno != EAGAIN)
        return luaL_error(L, "%s", strerror(errno));

    lua_getfield(L, -1, "receive");
    lua_insert(L, -2); /* swap */

    assert(lua_status(L) == LUA_OK);
    lua_callk(L, 1, 2, 0, finish_read);

    return finish_read(L, 0, 0);
}
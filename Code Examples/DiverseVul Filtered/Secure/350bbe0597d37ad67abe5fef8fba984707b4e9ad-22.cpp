static int finish_read (lua_State *L, int status, lua_KContext ctx) {
    int rc;
    struct ssh_userdata *sshu = NULL;

    sshu = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");

    if (lua_toboolean(L, -2)) {
        size_t n = 0;
        size_t l = 0;
        lua_getuservalue(L, 1);
        lua_getfield(L, -1, "sp_buff");
        lua_pushvalue(L, 3);
        lua_concat(L, 2);
        const char *data = lua_tolstring(L, -1, &l);
        lua_pushliteral(L, "");
        lua_setfield(L, 4, "sp_buff");

        while (n < l) {
#ifdef WIN32
            rc = send(sshu->sp[1], data + n, l - n, 0);
#else
            rc = write(sshu->sp[1], data + n, l - n);
#endif
            if (rc == -1 && errno != EAGAIN) {
                luaL_error(L, "Writing to socket pair: %s", strerror(errno));
            }
            else if (rc == -1 && errno == EAGAIN) {
                lua_pushlstring(L, data + n, l - n);
                lua_setfield(L, 4, "sp_buff");
                break;
            }
            else {
                n += rc;
            }
        }
        return 0;
    }
    else {
        return lua_error(L); /* uses idx 6 */
    }
}
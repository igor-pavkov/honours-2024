static int gc (lua_State *L) {
    struct ssh_userdata *sshu = NULL;

    sshu = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");
    if (!sshu) { return 0; }
    if (sshu) {
        // lua_pushvalue(L, lua_upvalueindex(1));
        // lua_getfield(L, -1, "session_close");
        // lua_insert(L, -2); /* swap */
        // lua_pcall(L, 1, 0, 0); /* if an error occurs, don't do anything */

        if (sshu->session != NULL) {
            if (libssh2_session_free(sshu->session) < 0) {
                // Unable to free libssh2 session
            }
            sshu->session = NULL;
        }
    }

#ifdef WIN32
    closesocket(sshu->sp[0]);
    closesocket(sshu->sp[1]);
#else
    close(sshu->sp[0]);
    close(sshu->sp[1]);
#endif

    return 0;
}
static int ssh_error (lua_State *L, LIBSSH2_SESSION *session, const char *msg) {
    char *errmsg;
    libssh2_session_last_error(session, &errmsg, NULL, 0);

    return nseU_safeerror(L, "%s: %s", msg, errmsg);
}
static int l_hostkey_hash (lua_State *L) {
    luaL_Buffer B;
    static int hash_option[] = { LIBSSH2_HOSTKEY_HASH_MD5, LIBSSH2_HOSTKEY_HASH_SHA1 };
    static int hash_length[] = { 16, 20 };
    static const char *hashes[] = { "md5", "sha1", NULL };
    int type = luaL_checkoption(L, 2, "sha1", hashes);
    struct ssh_userdata *state = NULL;
    const unsigned char *hash = NULL;

    state = (struct ssh_userdata *) nseU_checkudata(L, 1, SSH2_UDATA, "ssh2");
    hash = (const unsigned char *) libssh2_hostkey_hash(state->session, hash_option[type]);

    if (hash == NULL)
        return nseU_safeerror(L, "could not get hostkey hash");

    luaL_buffinit(L, &B);
    for (int i = 0; i < hash_length[type]; i++) {
        char byte[3]; /* with space for NULL */
        snprintf(byte, sizeof(byte), "%02X", (unsigned int)hash[i]);
        if (i)
            luaL_addchar(&B, ':');
        luaL_addlstring(&B, byte, 2);
    }
    luaL_pushresult(&B);

    return 1;
}
static int l_read_publickey (lua_State *L) {
    FILE *fd;
    char c;
    const char* publickeyfile = luaL_checkstring(L, 1);
    luaL_Buffer publickey_data;

    fd = fopen(publickeyfile, "r");
    if (!fd)
        return luaL_error(L, "Error reading file");

    luaL_buffinit(L, &publickey_data);
    while (fread(&c, 1, 1, fd) && c!= '\r' && c != '\n' && c != ' ') {
        continue;
    }
    while (fread(&c, 1, 1, fd) && c!= '\r' && c != '\n' && c != ' ') {
        luaL_addchar(&publickey_data, c);
    }
    fclose(fd);

    lua_getglobal(L, "require");
    lua_pushstring(L, "base64");
    lua_call(L, 1, 1);
    lua_getfield(L, -1, "dec");

    luaL_pushresult(&publickey_data);
    lua_call(L, 1, 1);

    return 1;
}
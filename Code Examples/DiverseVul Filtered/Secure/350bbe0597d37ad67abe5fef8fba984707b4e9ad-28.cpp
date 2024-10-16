static int finish_send (lua_State *L, int status, lua_KContext ctx) {
    if (lua_toboolean(L, -2))
        return 0;
    else
        return lua_error(L); /* uses idx 6 */
}
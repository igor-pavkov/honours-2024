static int req_aprtable2luatable_cb(void *l, const char *key,
                                    const char *value)
{
    int t;
    lua_State *L = (lua_State *) l;     /* [table<s,t>, table<s,s>] */
    /* rstack_dump(L, RRR, "start of cb"); */
    /* L is [table<s,t>, table<s,s>] */
    /* build complex */

    lua_getfield(L, -1, key);   /* [VALUE, table<s,t>, table<s,s>] */
    /* rstack_dump(L, RRR, "after getfield"); */
    t = lua_type(L, -1);
    switch (t) {
    case LUA_TNIL:
    case LUA_TNONE:{
            lua_pop(L, 1);      /* [table<s,t>, table<s,s>] */
            lua_newtable(L);    /* [array, table<s,t>, table<s,s>] */
            lua_pushnumber(L, 1);       /* [1, array, table<s,t>, table<s,s>] */
            lua_pushstring(L, value);   /* [string, 1, array, table<s,t>, table<s,s>] */
            lua_settable(L, -3);        /* [array, table<s,t>, table<s,s>]  */
            lua_setfield(L, -2, key);   /* [table<s,t>, table<s,s>] */
            break;
        }
    case LUA_TTABLE:{
            /* [array, table<s,t>, table<s,s>] */
            int size = lua_rawlen(L, -1);
            lua_pushnumber(L, size + 1);        /* [#, array, table<s,t>, table<s,s>] */
            lua_pushstring(L, value);   /* [string, #, array, table<s,t>, table<s,s>] */
            lua_settable(L, -3);        /* [array, table<s,t>, table<s,s>] */
            lua_setfield(L, -2, key);   /* [table<s,t>, table<s,s>] */
            break;
        }
    }

    /* L is [table<s,t>, table<s,s>] */
    /* build simple */
    lua_getfield(L, -2, key);   /* [VALUE, table<s,s>, table<s,t>] */
    if (lua_isnoneornil(L, -1)) {       /* only set if not already set */
        lua_pop(L, 1);          /* [table<s,s>, table<s,t>]] */
        lua_pushstring(L, value);       /* [string, table<s,s>, table<s,t>] */
        lua_setfield(L, -3, key);       /* [table<s,s>, table<s,t>]  */
    }
    else {
        lua_pop(L, 1);
    }
    return 1;
}
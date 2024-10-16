static lu_mem propagatemark (global_State *g) {
  GCObject *o = g->gray;
  gray2black(o);
  g->gray = *getgclist(o);  /* remove from 'gray' list */
  switch (o->tt) {
    case LUA_VTABLE: return traversetable(g, gco2t(o));
    case LUA_VUSERDATA: return traverseudata(g, gco2u(o));
    case LUA_VLCL: return traverseLclosure(g, gco2lcl(o));
    case LUA_VCCL: return traverseCclosure(g, gco2ccl(o));
    case LUA_VPROTO: return traverseproto(g, gco2p(o));
    case LUA_VTHREAD: {
      lua_State *th = gco2th(o);
      linkgclist(th, g->grayagain);  /* insert into 'grayagain' list */
      black2gray(o);
      return traversethread(g, th);
    }
    default: lua_assert(0); return 0;
  }
}
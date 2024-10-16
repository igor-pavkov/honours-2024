static void freeobj (lua_State *L, GCObject *o) {
  switch (o->tt) {
    case LUA_VPROTO:
      luaF_freeproto(L, gco2p(o));
      break;
    case LUA_VUPVAL:
      freeupval(L, gco2upv(o));
      break;
    case LUA_VLCL:
      luaM_freemem(L, o, sizeLclosure(gco2lcl(o)->nupvalues));
      break;
    case LUA_VCCL:
      luaM_freemem(L, o, sizeCclosure(gco2ccl(o)->nupvalues));
      break;
    case LUA_VTABLE:
      luaH_free(L, gco2t(o));
      break;
    case LUA_VTHREAD:
      luaE_freethread(L, gco2th(o));
      break;
    case LUA_VUSERDATA: {
      Udata *u = gco2u(o);
      luaM_freemem(L, o, sizeudata(u->nuvalue, u->len));
      break;
    }
    case LUA_VSHRSTR:
      luaS_remove(L, gco2ts(o));  /* remove it from hash table */
      luaM_freemem(L, o, sizelstring(gco2ts(o)->shrlen));
      break;
    case LUA_VLNGSTR:
      luaM_freemem(L, o, sizelstring(gco2ts(o)->u.lnglen));
      break;
    default: lua_assert(0);
  }
}
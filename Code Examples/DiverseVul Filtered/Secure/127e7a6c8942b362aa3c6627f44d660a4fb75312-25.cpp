void luaC_step (lua_State *L) {
  global_State *g = G(L);
  lua_assert(!g->gcemergency);
  if (g->gcrunning) {  /* running? */
    if(isdecGCmodegen(g))
      genstep(L, g);
    else
      incstep(L, g);
  }
}
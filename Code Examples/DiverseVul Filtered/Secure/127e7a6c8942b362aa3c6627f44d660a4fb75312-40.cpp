static void stepgenfull (lua_State *L, global_State *g) {
  lu_mem newatomic;  /* count of traversed objects */
  lu_mem lastatomic = g->lastatomic;  /* count from last collection */
  if (g->gckind == KGC_GEN)  /* still in generational mode? */
    enterinc(g);  /* enter incremental mode */
  luaC_runtilstate(L, bitmask(GCSpropagate));  /* start new cycle */
  newatomic = atomic(L);  /* mark everybody */
  if (newatomic < lastatomic + (lastatomic >> 3)) {  /* good collection? */
    atomic2gen(L, g);  /* return to generational mode */
    setminordebt(g);
  }
  else {  /* another bad collection; stay in incremental mode */
    g->GCestimate = gettotalbytes(g);  /* first estimate */;
    entersweep(L);
    luaC_runtilstate(L, bitmask(GCSpause));  /* finish collection */
    setpause(g);
    g->lastatomic = newatomic;
  }
}
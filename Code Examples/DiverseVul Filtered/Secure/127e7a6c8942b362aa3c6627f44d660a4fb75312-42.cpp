void luaC_barrierback_ (lua_State *L, GCObject *o) {
  global_State *g = G(L);
  lua_assert(isblack(o) && !isdead(g, o));
  lua_assert(g->gckind != KGC_GEN || (isold(o) && getage(o) != G_TOUCHED1));
  if (getage(o) != G_TOUCHED2)  /* not already in gray list? */
    linkobjgclist(o, g->grayagain);  /* link it in 'grayagain' */
  black2gray(o);  /* make object gray (again) */
  setage(o, G_TOUCHED1);  /* touched in current cycle */
}
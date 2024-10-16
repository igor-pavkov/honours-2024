Lua::Lua() {
  L = luaL_newstate();

  if(L == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Unable to create Lua interpreter");
    return;
  }
}
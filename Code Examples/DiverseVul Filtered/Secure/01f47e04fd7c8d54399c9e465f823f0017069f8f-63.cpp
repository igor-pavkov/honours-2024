static int ntop_swapHostBlacklist(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  ntop->swapHostBlacklist();
  lua_pushnil(vm);
  return(CONST_LUA_OK);
}
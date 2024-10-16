static int ntop_allocHostBlacklist(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  ntop->allocHostBlacklist();
  lua_pushnil(vm);
  return(CONST_LUA_OK);
}
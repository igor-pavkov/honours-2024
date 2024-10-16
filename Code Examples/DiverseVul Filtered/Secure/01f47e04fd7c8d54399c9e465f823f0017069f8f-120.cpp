static int ntop_get_uptime(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_pushinteger(vm, ntop->getGlobals()->getUptime());
  return(CONST_LUA_OK);
}
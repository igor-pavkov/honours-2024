static int ntop_get_allowed_networks(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  ntop->getAllowedNetworks(vm);
  return(CONST_LUA_OK);
}
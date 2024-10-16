static int ntop_get_interface_flows_stats(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  if(ntop_interface) ntop_interface->getFlowsStats(vm);

  return(CONST_LUA_OK);
}
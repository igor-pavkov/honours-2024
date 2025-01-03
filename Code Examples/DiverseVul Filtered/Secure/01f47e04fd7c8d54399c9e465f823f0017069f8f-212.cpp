static int ntop_get_ndpi_interface_flows_status(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface) {
    lua_newtable(vm);
    ntop_interface->getFlowsStatus(vm);
  }

  return(CONST_LUA_OK);
}
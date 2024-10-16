static int ntop_get_interface_dump_max_pkts(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  int max_pkts;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  max_pkts = ntop_interface->getDumpTrafficMaxPktsPerFile();

  lua_pushnumber(vm, max_pkts);

  return(CONST_LUA_OK);
}
static int ntop_set_second_traffic(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface) return(CONST_LUA_ERROR);
  ntop_interface->updateSecondTraffic(time(NULL));

  return(CONST_LUA_OK);
}
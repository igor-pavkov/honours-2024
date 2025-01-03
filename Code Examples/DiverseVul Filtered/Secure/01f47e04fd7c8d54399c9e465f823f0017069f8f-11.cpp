static int ntop_get_interface_dump_tap_policy(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool dump_traffic_to_tap;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  dump_traffic_to_tap = ntop_interface->getDumpTrafficTapPolicy();

  lua_pushboolean(vm, dump_traffic_to_tap ? 1 : 0);

  return(CONST_LUA_OK);
}
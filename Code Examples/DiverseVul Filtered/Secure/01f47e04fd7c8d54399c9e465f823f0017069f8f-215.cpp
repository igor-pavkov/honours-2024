static int ntop_get_interface_dump_disk_policy(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool dump_traffic_to_disk;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  dump_traffic_to_disk = ntop_interface->getDumpTrafficDiskPolicy();

  lua_pushboolean(vm, dump_traffic_to_disk ? 1 : 0);

  return(CONST_LUA_OK);
}
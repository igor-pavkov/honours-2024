static int ntop_get_interface_pkts_dumped_file(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  int num_pkts;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  PacketDumper *dumper = ntop_interface->getPacketDumper();
  if(!dumper)
    return(CONST_LUA_ERROR);

  num_pkts = dumper->get_num_dumped_packets();

  lua_pushnumber(vm, num_pkts);

  return(CONST_LUA_OK);
}
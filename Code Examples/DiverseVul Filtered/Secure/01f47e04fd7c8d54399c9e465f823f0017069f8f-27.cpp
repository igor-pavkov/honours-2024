static int ntop_interface_is_packet_interface(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface) return(CONST_LUA_ERROR);

  lua_pushboolean(vm, ntop_interface->isPacketInterface());
  return(CONST_LUA_OK);
}
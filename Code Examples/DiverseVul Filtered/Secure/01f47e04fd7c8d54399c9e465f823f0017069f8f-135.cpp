static int ntop_has_vlans(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface)
    lua_pushboolean(vm, ntop_interface->hasSeenVlanTaggedPackets());
  else
    lua_pushboolean(vm, 0);

  return(CONST_LUA_OK);
}
static int ntop_interface_is_bridge_interface(lua_State* vm) {
  int ifid;
  NetworkInterface *iface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if((lua_type(vm, 1) == LUA_TNUMBER)) {
    ifid = lua_tointeger(vm, 1);

    if(ifid < 0 || !(iface = ntop->getNetworkInterface(ifid)))
      return (CONST_LUA_ERROR);
  }

  lua_pushboolean(vm, iface->is_bridge_interface());
  return(CONST_LUA_OK);
}
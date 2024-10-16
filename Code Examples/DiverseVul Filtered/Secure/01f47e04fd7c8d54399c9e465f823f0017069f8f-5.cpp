static int ntop_get_default_interface_name(lua_State* vm) {
  char ifname[MAX_INTERFACE_NAME_LEN];
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop->getInterfaceAllowed(vm, ifname)) {
    // if there is an allowed interface for the user
    // we return that interface
    lua_pushstring(vm,
		   ntop->getNetworkInterface(ifname)->get_name());
  } else {
    lua_pushstring(vm, ntop->getInterfaceAtId(NULL, /* no need to check as there is no constaint */
					      0)->get_name());
  }
  return(CONST_LUA_OK);
}
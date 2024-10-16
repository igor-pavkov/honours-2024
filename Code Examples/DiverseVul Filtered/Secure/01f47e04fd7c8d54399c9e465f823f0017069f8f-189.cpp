static int ntop_getflowdevices(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);
  else {
    ntop_interface->getFlowDevices(vm);
    return(CONST_LUA_OK);
  }
}
static int ntop_getflowdeviceinfo(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *device_ip;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  device_ip = (char*)lua_tostring(vm, 1);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);
  else {
    in_addr_t addr = inet_addr(device_ip);

    ntop_interface->getFlowDeviceInfo(vm, ntohl(addr));
    return(CONST_LUA_OK);
  }
}
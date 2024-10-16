static int ntop_get_host_pool_volatile_members(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  nDPIStats stats;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface && ntop_interface->getHostPools()) {
    ntop_interface->luaHostPoolsVolatileMembers(vm);
    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);

}
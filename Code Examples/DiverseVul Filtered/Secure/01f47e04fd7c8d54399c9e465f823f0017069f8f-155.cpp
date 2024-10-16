static int ntop_purge_expired_host_pools_members(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface && ntop_interface->getHostPools()) {

    ntop_interface->getHostPools()->purgeExpiredVolatileMembers();

    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
static int ntop_dump_local_hosts_2_redis(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  ntop_interface->dumpLocalHosts2redis(true /* must disable purge as we are called from lua */);

  return(CONST_LUA_OK);
}
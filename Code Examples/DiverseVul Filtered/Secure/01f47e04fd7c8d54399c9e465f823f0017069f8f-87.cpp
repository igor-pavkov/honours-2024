static int ntop_reload_host_pools(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface) {

    ntop_interface->getHostPools()->reloadPools();

    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
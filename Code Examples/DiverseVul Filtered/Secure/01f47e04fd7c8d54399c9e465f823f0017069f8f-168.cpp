static int ntop_reload_traffic_profiles(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface)
    ntop_interface->updateFlowProfiles(); /* Reload profiles in memory */

  lua_pushnil(vm);
  return(CONST_LUA_OK);
}
static int ntop_interface_reset_counters(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool only_drops = true;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TBOOLEAN)
    only_drops = lua_toboolean(vm, 1) ? true : false;

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  ntop_interface->checkPointCounters(only_drops);
  return(CONST_LUA_OK);
}
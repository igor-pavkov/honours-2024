static int ntop_interface_set_idle(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool state;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TBOOLEAN)) return(CONST_LUA_ERROR);
  state = lua_toboolean(vm, 1) ? true : false;

  ntop_interface->setIdleState(state);

  return(CONST_LUA_OK);
}
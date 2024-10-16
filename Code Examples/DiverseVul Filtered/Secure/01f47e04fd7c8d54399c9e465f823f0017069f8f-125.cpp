static int ntop_get_interface_find_pid_flows(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  u_int32_t pid;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  pid = (u_int32_t)lua_tonumber(vm, 1);

  if(!ntop_interface) return(CONST_LUA_ERROR);

  ntop_interface->findPidFlows(vm, pid);
  return(CONST_LUA_OK);
}
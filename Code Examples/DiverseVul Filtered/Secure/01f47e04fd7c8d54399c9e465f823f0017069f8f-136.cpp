static int ntop_get_interface_find_father_pid_flows(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  u_int32_t father_pid;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  father_pid = (u_int32_t)lua_tonumber(vm, 1);

  if(!ntop_interface) return(CONST_LUA_ERROR);

  ntop_interface->findFatherPidFlows(vm, father_pid);
  return(CONST_LUA_OK);
}
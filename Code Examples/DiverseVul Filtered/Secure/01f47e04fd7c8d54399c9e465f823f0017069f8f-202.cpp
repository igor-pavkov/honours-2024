static int ntop_get_interface_find_host(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *key;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  key = (char*)lua_tostring(vm, 1);

  if(!ntop_interface) return(CONST_LUA_ERROR);
  ntop_interface->findHostsByName(vm, get_allowed_nets(vm), key);

  return(CONST_LUA_OK);
}
static int ntop_get_interface_dump_max_sec(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  int max_sec;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  max_sec = ntop_interface->getDumpTrafficMaxSecPerFile();

  lua_pushnumber(vm, max_sec);

  return(CONST_LUA_OK);
}
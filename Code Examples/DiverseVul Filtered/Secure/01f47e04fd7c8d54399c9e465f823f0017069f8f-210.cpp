static int ntop_get_interface_dump_max_files(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  int max_files;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  max_files = ntop_interface->getDumpTrafficMaxFiles();

  lua_pushnumber(vm, max_files);

  return(CONST_LUA_OK);
}
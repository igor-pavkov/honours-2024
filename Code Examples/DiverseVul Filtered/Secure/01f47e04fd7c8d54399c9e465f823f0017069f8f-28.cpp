static int ntop_get_interface_dump_tap_name(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  lua_pushstring(vm, ntop_interface->getDumpTrafficTapName());

  return(CONST_LUA_OK);
}
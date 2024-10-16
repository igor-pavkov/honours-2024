static NetworkInterface* getCurrentInterface(lua_State* vm) {
  NetworkInterface *ntop_interface;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_getglobal(vm, "ntop_interface");
  if((ntop_interface = (NetworkInterface*)lua_touserdata(vm, lua_gettop(vm))) == NULL) {
    ntop_interface = handle_null_interface(vm);
  }

  return(ntop_interface);
}
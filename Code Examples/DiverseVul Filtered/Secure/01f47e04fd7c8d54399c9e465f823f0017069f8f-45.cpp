static int ntop_load_dump_prefs(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  ntop_interface->loadDumpPrefs();

  return(CONST_LUA_OK);
}
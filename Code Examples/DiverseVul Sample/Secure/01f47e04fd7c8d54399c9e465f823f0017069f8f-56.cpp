static int ntop_nagios_reload_config(lua_State* vm) {
  NagiosManager *nagios = ntop->getNagios();

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  if(!nagios) {
      ntop->getTrace()->traceEvent(TRACE_ERROR, "%s(): unable to get the nagios manager",
				 __FUNCTION__);
    return(CONST_LUA_ERROR);
  }
  nagios->loadConfig();
  lua_pushnil(vm);
  return(CONST_LUA_OK);
}
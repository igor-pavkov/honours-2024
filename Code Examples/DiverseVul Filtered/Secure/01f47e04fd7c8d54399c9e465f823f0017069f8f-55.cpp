static int ntop_nagios_send_alert(lua_State* vm) {
  NagiosManager *nagios = ntop->getNagios();
  char *alert_source;
  char *timespan;
  char *alarmed_metric;
  char *alert_msg;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  alert_source = (char*)lua_tostring(vm, 1);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  timespan = (char*)lua_tostring(vm, 2);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING)) return(CONST_LUA_ERROR);
  alarmed_metric = (char*)lua_tostring(vm, 3);

  if(ntop_lua_check(vm, __FUNCTION__, 4, LUA_TSTRING)) return(CONST_LUA_ERROR);
  alert_msg = (char*)lua_tostring(vm, 4);

  nagios->sendAlert(alert_source, timespan, alarmed_metric, alert_msg);

  lua_pushnil(vm);
  return(CONST_LUA_OK);
}
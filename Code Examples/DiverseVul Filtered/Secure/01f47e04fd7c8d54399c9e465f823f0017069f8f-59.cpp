static int ntop_interface_get_cached_num_alerts(lua_State* vm) {
  NetworkInterface *iface = getCurrentInterface(vm);
  AlertsManager *am;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!iface || !(am = iface->getAlertsManager()))
    return (CONST_LUA_ERROR);

  return (!am->getCachedNumAlerts(vm)) ? CONST_LUA_OK : CONST_LUA_ERROR;
}
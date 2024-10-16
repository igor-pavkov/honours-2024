static int ntop_interface_store_alert(lua_State* vm) {
  int ifid;
  NetworkInterface* iface;
  AlertsManager *am;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TTABLE)) return(CONST_LUA_ERROR);

  ifid = lua_tointeger(vm, 1);
  if(ifid < 0)
    return(CONST_LUA_ERROR);

  if(!(iface = ntop->getNetworkInterface(vm, ifid)) ||
     !(am = iface->getAlertsManager()))
    return (CONST_LUA_ERROR);

  return am->storeAlert(vm, 2) ? CONST_LUA_ERROR : CONST_LUA_OK;
}
static int ntop_stats_delete_hour_older_than(lua_State *vm) {
  int num_days;
  int ifid;
  NetworkInterface* iface;
  StatsManager *sm;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  ifid = lua_tointeger(vm, 1);
  if(ifid < 0)
    return(CONST_LUA_ERROR);
  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  num_days = lua_tointeger(vm, 2);
  if(num_days < 0)
    return(CONST_LUA_ERROR);

  if(!(iface = ntop->getNetworkInterface(ifid)) ||
     !(sm = iface->getStatsManager()))
    return (CONST_LUA_ERROR);

  if(sm->deleteHourStatsOlderThan(num_days))
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
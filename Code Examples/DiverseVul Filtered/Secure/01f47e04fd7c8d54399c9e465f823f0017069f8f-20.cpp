static int ntop_stats_insert_hour_sampling(lua_State *vm) {
  char *sampling;
  time_t rawtime;
  int ifid;
  NetworkInterface* iface;
  StatsManager *sm;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  ifid = lua_tointeger(vm, 1);
  if(ifid < 0)
    return(CONST_LUA_ERROR);
  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((sampling = (char*)lua_tostring(vm, 2)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if(!(iface = ntop->getNetworkInterface(ifid)) ||
     !(sm = iface->getStatsManager()))
    return (CONST_LUA_ERROR);

  time(&rawtime);
  rawtime -= (rawtime % 60);

  if(sm->insertHourSampling(rawtime, sampling))
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
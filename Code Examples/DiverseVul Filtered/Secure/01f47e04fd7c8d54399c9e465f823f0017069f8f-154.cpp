static int ntop_stats_get_minute_sampling(lua_State *vm) {
  time_t epoch;
  string sampling;
  int ifid;
  NetworkInterface* iface;
  StatsManager *sm;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  ifid = lua_tointeger(vm, 1);
  if(ifid < 0)
    return(CONST_LUA_ERROR);
  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  epoch = (time_t)lua_tointeger(vm, 2);

  if(!(iface = ntop->getNetworkInterface(ifid)) ||
     !(sm = iface->getStatsManager()))
    return (CONST_LUA_ERROR);

  if(sm->getMinuteSampling(epoch, &sampling))
    return(CONST_LUA_ERROR);

  lua_pushstring(vm, sampling.c_str());

  return(CONST_LUA_OK);
}
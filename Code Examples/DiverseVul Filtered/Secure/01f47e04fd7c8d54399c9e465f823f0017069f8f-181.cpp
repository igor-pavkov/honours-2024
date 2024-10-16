static int ntop_stats_get_samplings_of_days_from_epoch(lua_State *vm) {
  time_t epoch_start, epoch_end;
  int num_days;
  int ifid;
  NetworkInterface* iface;
  StatsManager *sm;
  struct statsManagerRetrieval retvals;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  ifid = lua_tointeger(vm, 1);
  if(ifid < 0)
    return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  epoch_end = lua_tointeger(vm, 2);
  epoch_end -= (epoch_end % 60);
  if(epoch_end < 0)
    return(CONST_LUA_ERROR);
  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  num_days = lua_tointeger(vm, 3);
  if(num_days < 0)
    return(CONST_LUA_ERROR);

  if(!(iface = ntop->getNetworkInterface(ifid)) ||
     !(sm = iface->getStatsManager()))
    return (CONST_LUA_ERROR);

  epoch_start = epoch_end - (num_days * 24 * 60 * 60);

  if(sm->retrieveDayStatsInterval(epoch_start, epoch_end, &retvals))
    return(CONST_LUA_ERROR);

  lua_newtable(vm);

  for (unsigned i = 0 ; i < retvals.rows.size() ; i++)
    lua_push_str_table_entry(vm, retvals.rows[i].c_str(), (char*)"");

  return(CONST_LUA_OK);
}
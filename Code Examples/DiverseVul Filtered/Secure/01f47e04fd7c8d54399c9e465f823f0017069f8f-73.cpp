static int ntop_redis_get_id_to_host(lua_State* vm) {
  char *host_idx, rsp[CONST_MAX_LEN_REDIS_VALUE];
  Redis *redis = ntop->getRedis();
  char daybuf[32];
  time_t when = time(NULL);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((host_idx = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  strftime(daybuf, sizeof(daybuf), CONST_DB_DAY_FORMAT, localtime(&when));
  lua_pushfstring(vm, "%d", redis->id_to_host(daybuf, host_idx, rsp, sizeof(rsp)));

  return(CONST_LUA_OK);
}
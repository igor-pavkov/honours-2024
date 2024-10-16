static int ntop_redis_get_host_id(lua_State* vm) {
  char *host_name;
  Redis *redis = ntop->getRedis();
  char daybuf[32];
  time_t when = time(NULL);
  bool new_key;
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((host_name = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  strftime(daybuf, sizeof(daybuf), CONST_DB_DAY_FORMAT, localtime(&when));
  lua_pushinteger(vm, redis->host_to_id(ntop_interface, daybuf, host_name, &new_key)); /* CHECK */

  return(CONST_LUA_OK);
}
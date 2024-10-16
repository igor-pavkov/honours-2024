static int ntop_set_redis(lua_State* vm) {
  char *key, *value;
  u_int expire_secs = 0;  // default 0 = no expiration
  Redis *redis = ntop->getRedis();

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((key = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((value = (char*)lua_tostring(vm, 2)) == NULL)     return(CONST_LUA_PARAM_ERROR);

  /* Optional key expiration in SECONDS */
  if(lua_type(vm, 3) == LUA_TNUMBER)
      expire_secs = (u_int)lua_tonumber(vm, 3);

  if(redis->set(key, value, expire_secs) == 0) {
      return(CONST_LUA_OK);
  }else
      return(CONST_LUA_ERROR);
}
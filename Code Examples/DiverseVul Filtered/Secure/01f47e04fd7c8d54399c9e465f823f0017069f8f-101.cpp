static int ntop_get_hash_redis(lua_State* vm) {
  char *key, *member, rsp[CONST_MAX_LEN_REDIS_VALUE];
  Redis *redis = ntop->getRedis();

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((key = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);
  if((member = (char*)lua_tostring(vm, 2)) == NULL)    return(CONST_LUA_PARAM_ERROR);

  lua_pushfstring(vm, "%s", (redis->hashGet(key, member, rsp, sizeof(rsp)) == 0) ? rsp : (char*)"");

  return(CONST_LUA_OK);
}
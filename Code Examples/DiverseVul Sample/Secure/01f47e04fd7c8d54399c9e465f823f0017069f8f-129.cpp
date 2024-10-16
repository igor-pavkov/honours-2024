static int ntop_get_redis_set_pop(lua_State* vm) {
  char *set_name, rsp[CONST_MAX_LEN_REDIS_VALUE];
  Redis *redis = ntop->getRedis();

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((set_name = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);
  lua_pushfstring(vm, "%s", redis->popSet(set_name, rsp, sizeof(rsp)));

  return(CONST_LUA_OK);
}
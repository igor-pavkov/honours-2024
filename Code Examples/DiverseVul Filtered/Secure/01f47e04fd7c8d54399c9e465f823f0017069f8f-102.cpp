static int ntop_list_index_redis(lua_State* vm) {
  char *index_name, rsp[CONST_MAX_LEN_REDIS_VALUE];
  Redis *redis = ntop->getRedis();
  int idx;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING))  return(CONST_LUA_ERROR);
  if((index_name = (char*)lua_tostring(vm, 1)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  idx = lua_tointeger(vm, 2);

  if(redis->lindex(index_name, idx, rsp, sizeof(rsp)) != 0)
    return(CONST_LUA_ERROR);

  lua_pushfstring(vm, "%s", rsp);

  return(CONST_LUA_OK);
}
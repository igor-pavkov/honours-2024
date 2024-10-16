static int ntop_get_redis(lua_State* vm) {
  char *key, *rsp;
  u_int rsp_len = 32768;
  Redis *redis = ntop->getRedis();
  bool cache_it = false;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((key = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);

  /* Optional cache_it */
  if(lua_type(vm, 2) == LUA_TBOOLEAN) cache_it = lua_toboolean(vm, 2);


  if((rsp = (char*)malloc(rsp_len)) != NULL) {
    lua_pushfstring(vm, "%s", (redis->get(key, rsp, rsp_len, cache_it) == 0) ? rsp : (char*)"");
    free(rsp);
    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
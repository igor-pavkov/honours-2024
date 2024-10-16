static int ntop_lpop_redis(lua_State* vm) {
  char msg[1024], *list_name;
  Redis *redis = ntop->getRedis();

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((list_name = (char*)lua_tostring(vm, 1)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(redis->lpop(list_name, msg, sizeof(msg)) == 0) {
    lua_pushfstring(vm, "%s", msg);
    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
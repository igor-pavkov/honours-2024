static int ntop_get_set_members_redis(lua_State* vm) {
  char *key;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((key = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);
  ntop->getRedis()->smembers(vm, key);
  return(CONST_LUA_OK);
}
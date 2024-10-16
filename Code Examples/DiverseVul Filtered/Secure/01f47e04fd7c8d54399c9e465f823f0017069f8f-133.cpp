static int ntop_lpush_redis(lua_State* vm) {
  char *list_name, *value;
  u_int list_trim_size = 0;  // default 0 = no trim
  Redis *redis = ntop->getRedis();

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((list_name = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((value = (char*)lua_tostring(vm, 2)) == NULL)     return(CONST_LUA_PARAM_ERROR);

  /* Optional trim list up to the specified number of elements */
  if(lua_type(vm, 3) == LUA_TNUMBER)
      list_trim_size = (u_int)lua_tonumber(vm, 3);

  if(redis->lpush(list_name, value, list_trim_size) == 0) {
      return(CONST_LUA_OK);
  }else
      return(CONST_LUA_ERROR);
}
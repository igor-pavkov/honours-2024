static int ntop_change_user_host_pool(lua_State* vm) {
  char *username, *host_pool_id;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  if(!Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((username = (char*)lua_tostring(vm, 1)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((host_pool_id = (char*)lua_tostring(vm, 2)) == NULL) return(CONST_LUA_PARAM_ERROR);

  return ntop->changeUserHostPool(username, host_pool_id);
}
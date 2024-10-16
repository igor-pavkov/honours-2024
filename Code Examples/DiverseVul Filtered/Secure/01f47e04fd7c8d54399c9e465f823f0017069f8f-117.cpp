static int ntop_add_user(lua_State* vm) {
  char *username, *full_name, *password, *host_role, *allowed_networks, *allowed_interface, *host_pool_id = NULL;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((username = (char*)lua_tostring(vm, 1)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((full_name = (char*)lua_tostring(vm, 2)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((password = (char*)lua_tostring(vm, 3)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 4, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((host_role = (char*)lua_tostring(vm, 4)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 5, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((allowed_networks = (char*)lua_tostring(vm, 5)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 6, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((allowed_interface = (char*)lua_tostring(vm, 6)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(lua_type(vm, 7) == LUA_TSTRING)
    if((host_pool_id = (char*)lua_tostring(vm, 7)) == NULL) return(CONST_LUA_PARAM_ERROR);

  return ntop->addUser(username, full_name, password, host_role,
		       allowed_networks, allowed_interface, host_pool_id);
}
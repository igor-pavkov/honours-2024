static int ntop_add_user_lifetime(lua_State* vm) {
  char *username;
  int32_t num_secs;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((username = (char*)lua_tostring(vm, 1)) == NULL) return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_PARAM_ERROR);
  num_secs = (int32_t)lua_tonumber(vm, 2);

  if(num_secs > 0)
    return ntop->addUserLifetime(username, num_secs) ? CONST_LUA_OK : CONST_LUA_ERROR;

  return CONST_LUA_OK; /* Negative or zero lifetimes means unlimited */
}
static int ntop_syslog(lua_State* vm) {
#ifndef WIN32
  char *msg;
  bool is_error;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TBOOLEAN)) return(CONST_LUA_ERROR);
  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING))  return(CONST_LUA_ERROR);

  is_error = lua_toboolean(vm, 1) ? true : false;
  msg = (char*)lua_tostring(vm, 2);

  syslog(is_error ? LOG_ERR : LOG_INFO, "%s", msg);
#endif

  return(CONST_LUA_OK);
}
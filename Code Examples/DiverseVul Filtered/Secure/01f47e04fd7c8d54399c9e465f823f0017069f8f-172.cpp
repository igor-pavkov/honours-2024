static int ntop_check_profile_syntax(lua_State* vm) {
  char *filter;
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  filter = (char*)lua_tostring(vm, 1);

  lua_pushboolean(vm, ntop_interface ? ntop_interface->checkProfileSyntax(filter) : false);

  return(CONST_LUA_OK);
}
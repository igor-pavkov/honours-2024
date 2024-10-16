static int ntop_addToHostBlacklist(lua_State* vm) {
  char *net;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  net = (char*)lua_tostring(vm, 1);

  ntop->addToHostBlacklist(net);
  lua_pushnil(vm);
  return(CONST_LUA_OK);
}
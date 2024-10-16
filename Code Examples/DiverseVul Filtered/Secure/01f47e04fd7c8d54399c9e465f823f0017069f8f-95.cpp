static int ntop_get_users(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  ntop->getUsers(vm);
  return(CONST_LUA_OK);
}
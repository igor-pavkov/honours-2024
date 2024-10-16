static int ntop_get_user_group(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  ntop->getUserGroup(vm);
  return(CONST_LUA_OK);
}
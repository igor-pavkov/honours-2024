static int ntop_is_enterprise(lua_State *vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);
  lua_pushboolean(vm, ntop->getPrefs()->is_enterprise_edition());
  return(CONST_LUA_OK);
}
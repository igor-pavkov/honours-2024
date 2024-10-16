static int ntop_get_prefs(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  ntop->getPrefs()->lua(vm);

  return(CONST_LUA_OK);
}
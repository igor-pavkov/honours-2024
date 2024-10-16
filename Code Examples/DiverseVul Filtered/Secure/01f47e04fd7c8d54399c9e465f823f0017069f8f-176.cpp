static int ntop_http_get_prefix(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_pushstring(vm, ntop->getPrefs()->get_http_prefix());
  return(CONST_LUA_OK);
}
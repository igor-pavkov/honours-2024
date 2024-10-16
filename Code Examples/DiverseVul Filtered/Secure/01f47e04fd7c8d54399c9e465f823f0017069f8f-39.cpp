static int ntop_has_geoip(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_pushboolean(vm, ntop->getGeolocation() ? 1 : 0);
  return(CONST_LUA_OK);
}
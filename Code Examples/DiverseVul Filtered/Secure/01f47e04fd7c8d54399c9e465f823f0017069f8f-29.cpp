static int ntop_get_nologin_username(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_pushstring(vm, NTOP_NOLOGIN_USER);

  return(CONST_LUA_OK);
}
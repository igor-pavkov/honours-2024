static int ntop_is_login_disabled(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  bool ret = ntop->getPrefs()->is_localhost_users_login_disabled()
    || !ntop->getPrefs()->is_users_login_enabled();

  lua_pushboolean(vm, ret);

  return(CONST_LUA_OK);
}
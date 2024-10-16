static int ntop_check_license(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

#ifdef NTOPNG_PRO
  ntop->getPro()->check_license();
#endif

  lua_pushinteger(vm,1);
  return(CONST_LUA_OK);
}
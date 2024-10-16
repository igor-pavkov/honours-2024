static int ntop_gettimemsec(lua_State* vm) {
  struct timeval tp;
  double ret;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  gettimeofday(&tp, NULL);

  ret = (((double)tp.tv_usec) / (double)1000) + tp.tv_sec;

  lua_pushnumber(vm, ret);
  return(CONST_LUA_OK);
}
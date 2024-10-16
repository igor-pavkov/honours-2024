static int ntop_is_windows(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_pushboolean(vm,
#ifdef WIN32
		  1
#else
		  0
#endif
		  );

  return(CONST_LUA_OK);
}
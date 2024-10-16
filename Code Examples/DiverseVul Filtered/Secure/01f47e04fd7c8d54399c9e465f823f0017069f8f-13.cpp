static int ntop_reload_shapers(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface) {
#ifdef NTOPNG_PRO
    ntop_interface->refreshShapers();
#endif
    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
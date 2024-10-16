static int ntop_reload_l7_rules(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_PARAM_ERROR);

  if(ntop_interface) {
#ifdef NTOPNG_PRO
    u_int16_t host_pool_id = (u_int16_t)lua_tonumber(vm, 1);

#ifdef SHAPER_DEBUG
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "%s(%i)", __FUNCTION__, host_pool_id);
#endif

    ntop_interface->refreshL7Rules();
    ntop_interface->updateHostsL7Policy(host_pool_id);
    ntop_interface->updateFlowsL7Policy();
#endif

    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
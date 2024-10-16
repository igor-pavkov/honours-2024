static int ntop_remove_volatile_member_from_pool(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *host_or_mac;
  u_int16_t pool_id;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((host_or_mac = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER)) return(CONST_LUA_PARAM_ERROR);
  pool_id = (u_int16_t)lua_tonumber(vm, 2);

  if(ntop_interface && ntop_interface->getHostPools()) {

    ntop_interface->getHostPools()->removeVolatileMemberFromPool(host_or_mac, pool_id);

    return(CONST_LUA_OK);
  } else
    return(CONST_LUA_ERROR);
}
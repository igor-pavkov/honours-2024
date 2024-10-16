static int ntop_get_ndpi_protocol_id(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  nDPIStats stats;
  char *proto;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  proto = (char*)lua_tostring(vm, 1);

  if(ntop_interface && proto)
    lua_pushnumber(vm, ntop_interface->get_ndpi_proto_id(proto));
  else
    lua_pushnil(vm);

  return(CONST_LUA_OK);
}
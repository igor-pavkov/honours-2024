static int ntop_set_active_interface_id(lua_State* vm) {
  NetworkInterface *iface;
  int id;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  id = (u_int32_t)lua_tonumber(vm, 1);

  iface = ntop->getNetworkInterface(vm, id);

  ntop->getTrace()->traceEvent(TRACE_INFO, "Index: %d, Name: %s", id, iface ? iface->get_name() : "<unknown>");

  if(iface != NULL)
    lua_pushstring(vm, iface->get_name());
  else
    lua_pushnil(vm);

  return(CONST_LUA_OK);
}
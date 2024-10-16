static int ntop_network_name_by_id(lua_State* vm) {
  int id;
  char *name;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  id = (u_int32_t)lua_tonumber(vm, 1);

  name = ntop->getLocalNetworkName(id);

  lua_pushstring(vm, name ? name : "");

  return(CONST_LUA_OK);
}
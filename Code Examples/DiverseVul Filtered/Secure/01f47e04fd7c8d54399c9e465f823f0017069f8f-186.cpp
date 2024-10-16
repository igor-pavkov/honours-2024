static int ntop_interface_name2id(lua_State* vm) {
  char *if_name;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TNIL)
    if_name = NULL;
  else {
    if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
    if_name = (char*)lua_tostring(vm, 1);
  }

  lua_pushinteger(vm, ntop->getInterfaceIdByName(if_name));

  return(CONST_LUA_OK);
}
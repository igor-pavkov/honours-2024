static int ntop_select_interface(lua_State* vm) {
  char *ifname;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TNIL)
    ifname = (char*)"any";
  else {
    if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
    ifname = (char*)lua_tostring(vm, 1);
  }

  lua_pushlightuserdata(vm, (char*)ntop->getNetworkInterface(vm, ifname));
  lua_setglobal(vm, "ntop_interface");

  return(CONST_LUA_OK);
}
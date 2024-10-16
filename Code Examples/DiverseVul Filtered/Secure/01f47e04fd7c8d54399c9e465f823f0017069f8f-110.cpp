static int ntop_get_grouped_interface_host(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *country_s = NULL, *os_s = NULL;
  u_int16_t vlan_n,    *vlan_ptr    = NULL;
  u_int32_t as_n,      *as_ptr      = NULL;
  int16_t   network_n, *network_ptr = NULL;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TNUMBER) vlan_n    = (u_int16_t)lua_tonumber(vm, 1), vlan_ptr  = &vlan_n;
  if(lua_type(vm, 2) == LUA_TNUMBER) as_n      = (u_int32_t)lua_tonumber(vm, 2), as_ptr    = &as_n;
  if(lua_type(vm, 3) == LUA_TNUMBER) network_n = (int16_t)lua_tonumber(vm, 3), network_ptr = &network_n;
  if(lua_type(vm, 4) == LUA_TSTRING) country_s = (char*)lua_tostring(vm, 4);
  if(lua_type(vm, 5) == LUA_TSTRING) os_s      = (char*)lua_tostring(vm, 5);

  if(!ntop_interface || ntop_interface->getActiveHostsGroup(vm, get_allowed_nets(vm), false, false, country_s, vlan_ptr, os_s, as_ptr,
							    network_ptr, (char*)"column_ip", (char*)"country", CONST_MAX_NUM_HITS, 0 /* toSkip */, true /* a2zSortOrder */) < 0)
    return(CONST_LUA_ERROR);
  else
    return(CONST_LUA_OK);
}
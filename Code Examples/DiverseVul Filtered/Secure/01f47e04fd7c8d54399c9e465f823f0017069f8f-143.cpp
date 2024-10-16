static int ntop_get_grouped_interface_hosts(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool show_details = true, hostsOnly = true;
  char *country = NULL, *os_filter = NULL;
  char *groupBy = (char*)"column_ip";
  u_int16_t vlan_filter,  *vlan_filter_ptr    = NULL;
  u_int32_t asn_filter,   *asn_filter_ptr     = NULL;
  u_int16_t pool_filter,  *pool_filter_ptr    = NULL;
  int16_t network_filter, *network_filter_ptr = NULL;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TBOOLEAN) show_details = lua_toboolean(vm, 1) ? true : false;
  if(lua_type(vm, 2) == LUA_TSTRING)  groupBy    = (char*)lua_tostring(vm, 2);
  if(lua_type(vm, 3) == LUA_TSTRING)  country = (char*)lua_tostring(vm, 3);
  if(lua_type(vm, 4) == LUA_TSTRING)  os_filter      = (char*)lua_tostring(vm, 4);
  if(lua_type(vm, 5) == LUA_TNUMBER)  vlan_filter    = (u_int16_t)lua_tonumber(vm, 5), vlan_filter_ptr = &vlan_filter;
  if(lua_type(vm, 6) == LUA_TNUMBER)  asn_filter     = (u_int32_t)lua_tonumber(vm, 6), asn_filter_ptr = &asn_filter;
  if(lua_type(vm, 7) == LUA_TNUMBER)  network_filter = (int16_t)lua_tonumber(vm, 7),  network_filter_ptr = &network_filter;
  if(lua_type(vm, 8) == LUA_TBOOLEAN) hostsOnly      = lua_toboolean(vm, 8) ? true : false;
  if(lua_type(vm, 9) == LUA_TNUMBER)  pool_filter    = (u_int16_t)lua_tonumber(vm, 9), pool_filter_ptr = &pool_filter;

  if((!ntop_interface)
     || ntop_interface->getActiveHostsGroup(vm, get_allowed_nets(vm),
					    show_details, location_all,
					    country,
					    vlan_filter_ptr, os_filter,
					    asn_filter_ptr, network_filter_ptr,
					    pool_filter_ptr,
					    hostsOnly, groupBy) < 0)
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
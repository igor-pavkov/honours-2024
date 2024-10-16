static int ntop_get_interface_hosts(lua_State* vm, LocationPolicy location) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool show_details = true;
  char *sortColumn = (char*)"column_ip", *country = NULL, *os_filter = NULL, *mac_filter = NULL;
  bool a2zSortOrder = true;
  u_int16_t vlan_filter,  *vlan_filter_ptr    = NULL;
  u_int32_t asn_filter,   *asn_filter_ptr     = NULL;
  int16_t network_filter, *network_filter_ptr = NULL;
  u_int16_t pool_filter, *pool_filter_ptr = NULL;
  u_int32_t toSkip = 0, maxHits = CONST_MAX_NUM_HITS;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TBOOLEAN) show_details   = lua_toboolean(vm, 1) ? true : false;
  if(lua_type(vm, 2) == LUA_TSTRING)  sortColumn     = (char*)lua_tostring(vm, 2);
  if(lua_type(vm, 3) == LUA_TNUMBER)  maxHits        = (u_int16_t)lua_tonumber(vm, 3);
  if(lua_type(vm, 4) == LUA_TNUMBER)  toSkip         = (u_int16_t)lua_tonumber(vm, 4);
  if(lua_type(vm, 5) == LUA_TBOOLEAN) a2zSortOrder   = lua_toboolean(vm, 5) ? true : false;
  if(lua_type(vm, 6) == LUA_TSTRING)  country        = (char*)lua_tostring(vm, 6);
  if(lua_type(vm, 7) == LUA_TSTRING)  os_filter      = (char*)lua_tostring(vm, 7);
  if(lua_type(vm, 8) == LUA_TNUMBER)  vlan_filter    = (u_int16_t)lua_tonumber(vm, 8), vlan_filter_ptr = &vlan_filter;
  if(lua_type(vm, 9) == LUA_TNUMBER)  asn_filter     = (u_int32_t)lua_tonumber(vm, 9), asn_filter_ptr = &asn_filter;
  if(lua_type(vm,10) == LUA_TNUMBER)  network_filter = (int16_t)lua_tonumber(vm, 10),  network_filter_ptr = &network_filter;
  if(lua_type(vm,11) == LUA_TSTRING)  mac_filter     = (char*)lua_tostring(vm, 11);
  if(lua_type(vm,12) == LUA_TNUMBER)  pool_filter    = (u_int16_t)lua_tonumber(vm, 12), pool_filter_ptr = &pool_filter;

  if(!ntop_interface ||
    ntop_interface->getActiveHostsList(vm, get_allowed_nets(vm),
                                       show_details, location,
                                       country, mac_filter,
				       vlan_filter_ptr, os_filter, asn_filter_ptr,
				       network_filter_ptr, pool_filter_ptr,
				       sortColumn, maxHits,
				       toSkip, a2zSortOrder) < 0)
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
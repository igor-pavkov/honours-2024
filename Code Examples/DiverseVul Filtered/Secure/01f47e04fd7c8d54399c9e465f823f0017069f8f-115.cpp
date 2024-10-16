static int ntop_get_interface_macs_info(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *sortColumn = (char*)"column_mac";
  u_int32_t toSkip = 0, maxHits = CONST_MAX_NUM_HITS;
  u_int16_t vlan_id = 0;
  bool a2zSortOrder = true,
    skipSpecialMacs = false, hostMacsOnly = false;

  if(lua_type(vm, 1) == LUA_TSTRING) {
    sortColumn = (char*)lua_tostring(vm, 1);

    if(lua_type(vm, 2) == LUA_TNUMBER) {
      maxHits = (u_int16_t)lua_tonumber(vm, 2);

      if(lua_type(vm, 3) == LUA_TNUMBER) {
	toSkip = (u_int16_t)lua_tonumber(vm, 3);

	if(lua_type(vm, 4) == LUA_TBOOLEAN) {
	  a2zSortOrder = lua_toboolean(vm, 4) ? true : false;

	  if(lua_type(vm, 5) == LUA_TNUMBER) {
	    vlan_id = (u_int16_t)lua_tonumber(vm, 5);

	    if(lua_type(vm, 6) == LUA_TBOOLEAN) {
	      skipSpecialMacs = lua_toboolean(vm, 6) ? true : false;
	    }
	    if(lua_type(vm, 7) == LUA_TBOOLEAN) {
	      hostMacsOnly = lua_toboolean(vm, 7) ? true : false;
	    }
	  }
	}
      }
    }
  }

  if(!ntop_interface ||
     ntop_interface->getActiveMacList(vm, vlan_id, skipSpecialMacs,
				      hostMacsOnly,
				      sortColumn, maxHits,
				      toSkip, a2zSortOrder) < 0)
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
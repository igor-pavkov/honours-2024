static int ntop_get_interface_mac_info(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *mac = NULL;
  u_int16_t vlan_id = 0;

  if(lua_type(vm, 1) == LUA_TSTRING) {
    mac = (char*)lua_tostring(vm, 1);

    if(lua_type(vm, 2) == LUA_TNUMBER) {
      vlan_id = (u_int16_t)lua_tonumber(vm, 2);
    }
  }

  if((!ntop_interface)
     || (!mac)
     || (!ntop_interface->getMacInfo(vm, mac, vlan_id)))
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
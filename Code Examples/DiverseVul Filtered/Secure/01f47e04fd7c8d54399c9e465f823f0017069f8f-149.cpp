static int ntop_restore_interface_host(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *host_ip;
  u_int16_t vlan_id = 0;
  bool skip_privileges_check = false;
  char buf[64];

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);



  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  get_host_vlan_info((char*)lua_tostring(vm, 1), &host_ip, &vlan_id, buf, sizeof(buf));

  /* make sure skip privileges check cannot be set from the web interface */
  if(lua_type(vm, 2) == LUA_TBOOLEAN) skip_privileges_check = lua_toboolean(vm, 2);

  if(!skip_privileges_check && !Utils::isUserAdministrator(vm)) return(CONST_LUA_ERROR);

  if((!ntop_interface) || !ntop_interface->restoreHost(host_ip, vlan_id))
    return(CONST_LUA_ERROR);
  else
    return(CONST_LUA_OK);
}
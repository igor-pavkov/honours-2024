static int ntop_get_interface_latest_activity_hosts_info(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  if(!ntop_interface) return(CONST_LUA_ERROR);
  ntop_interface->getLatestActivityHostsList(vm, get_allowed_nets(vm));

  return(CONST_LUA_OK);
}
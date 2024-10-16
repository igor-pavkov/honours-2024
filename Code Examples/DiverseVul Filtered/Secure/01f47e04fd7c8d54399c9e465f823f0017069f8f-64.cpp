static int ntop_get_interface_remote_hosts_info(lua_State* vm) {
  return(ntop_get_interface_hosts(vm, location_remote_only));
}
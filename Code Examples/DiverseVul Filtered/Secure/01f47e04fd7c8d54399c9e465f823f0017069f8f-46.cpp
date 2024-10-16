static int ntop_get_interface_local_hosts_info(lua_State* vm) {
  return(ntop_get_interface_hosts(vm, location_local_only));
}
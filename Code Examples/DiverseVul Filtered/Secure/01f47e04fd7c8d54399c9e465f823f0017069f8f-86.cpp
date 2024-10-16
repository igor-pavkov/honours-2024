static int ntop_interface_release_network_alert(lua_State* vm) {
  return ntop_interface_engage_release_network_alert(vm, false /* release */);
}
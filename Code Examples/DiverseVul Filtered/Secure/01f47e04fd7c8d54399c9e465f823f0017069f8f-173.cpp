static int ntop_interface_release_host_alert(lua_State* vm) {
  return ntop_interface_engage_release_host_alert(vm, false /* release */);
}
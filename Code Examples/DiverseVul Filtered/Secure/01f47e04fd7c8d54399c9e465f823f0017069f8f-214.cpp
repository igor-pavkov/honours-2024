static int ntop_interface_engage_host_alert(lua_State* vm) {
  return ntop_interface_engage_release_host_alert(vm, true /* engage */);
}
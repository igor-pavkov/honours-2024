static int ntop_interface_release_interface_alert(lua_State* vm) {
  return ntop_interface_engage_release_interface_alert(vm, false /* release */);
}
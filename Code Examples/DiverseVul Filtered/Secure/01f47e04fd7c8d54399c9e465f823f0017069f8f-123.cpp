static int ntop_interface_host_enable_alerts(lua_State* vm) {
  return ntop_interface_host_trigger_alerts(vm, true);
}
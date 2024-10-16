static int ntop_get_ndpi_interface_stats(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  nDPIStats stats;
  char *host_ip = NULL;
  u_int16_t vlan_id = 0;
  char buf[64];

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  /* Optional host */
  if(lua_type(vm, 1) == LUA_TSTRING) get_host_vlan_info((char*)lua_tostring(vm, 1), &host_ip, &vlan_id, buf, sizeof(buf));

  /* Optional VLAN id */
  if(lua_type(vm, 2) == LUA_TNUMBER) vlan_id = (u_int16_t)lua_tonumber(vm, 2);

  if(ntop_interface) {
    ntop_interface->getnDPIStats(&stats, get_allowed_nets(vm), host_ip, vlan_id);

    lua_newtable(vm);
    stats.lua(ntop_interface, vm);
  }

  return(CONST_LUA_OK);
}
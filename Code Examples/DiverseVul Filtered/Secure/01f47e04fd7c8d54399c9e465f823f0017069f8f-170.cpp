static int ntop_interface_refresh_num_alerts(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  AlertsManager *am;
  Host *h;
  char *host_ip;
  u_int16_t vlan_id = 0;
  u_int32_t num_alerts;
  char buf[128];

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if((!ntop_interface))
    return(CONST_LUA_ERROR);

  if(lua_type(vm, 1) == LUA_TSTRING) {
    get_host_vlan_info((char*)lua_tostring(vm, 1), &host_ip, &vlan_id, buf, sizeof(buf));

    /* Optional VLAN id */
    if(lua_type(vm, 2) == LUA_TNUMBER) vlan_id = (u_int16_t)lua_tonumber(vm, 2);

    if((h = ntop_interface->getHost(host_ip, vlan_id))) {

      if(lua_type(vm, 3) == LUA_TNUMBER) {
	num_alerts = (u_int32_t)lua_tonumber(vm, 3);
	h->setNumAlerts(num_alerts);
      } else {
	h->getNumAlerts(true /* From AlertsManager re-reads the values */);
      }
    }

  } else {

    if((am = ntop_interface->getAlertsManager()) == NULL)
      return(CONST_LUA_ERROR);

    am->refreshCachedNumAlerts();
  }

  return(CONST_LUA_OK);
}
static int ntop_interface_engage_release_host_alert(lua_State* vm, bool engage) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *host_ip;
  u_int16_t vlan_id = 0;
  char buf[64];
  Host *h;
  int alert_severity;
  int alert_type;
  char *alert_json, *engaged_alert_id;
  AlertsManager *am;
  int ret;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  get_host_vlan_info((char*)lua_tostring(vm, 1), &host_ip, &vlan_id, buf, sizeof(buf));

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  engaged_alert_id = (char*)lua_tostring(vm, 2);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  alert_type = (int)lua_tonumber(vm, 3);

  if(ntop_lua_check(vm, __FUNCTION__, 4, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  alert_severity = (int)lua_tonumber(vm, 4);

  if(ntop_lua_check(vm, __FUNCTION__, 5, LUA_TSTRING)) return(CONST_LUA_ERROR);
  alert_json = (char*)lua_tostring(vm, 5);

  if((!ntop_interface)
     || ((h = ntop_interface->findHostsByIP(get_allowed_nets(vm), host_ip, vlan_id)) == NULL)
     || ((am = ntop_interface->getAlertsManager()) == NULL))
    return(CONST_LUA_ERROR);

  if(engage)
    ret = am->engageHostAlert(h, engaged_alert_id,
			      (AlertType)alert_type, (AlertLevel)alert_severity, alert_json);
  else
    ret = am->releaseHostAlert(h, engaged_alert_id,
			       (AlertType)alert_type, (AlertLevel)alert_severity, alert_json);

  return ret >= 0 ? CONST_LUA_OK : CONST_LUA_ERROR;
}
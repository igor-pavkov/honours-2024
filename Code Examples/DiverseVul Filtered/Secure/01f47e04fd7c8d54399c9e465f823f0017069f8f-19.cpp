static int ntop_get_interface_stats(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);

  /*
  ntop_interface->getAlertsManager()->engageAlert(alert_entity_host, "127.0.0.1",
						  "min_bytes",
						  alert_threshold_exceeded,
						  alert_level_warning,
						  "miao");
  ntop_interface->getAlertsManager()->releaseAlert(alert_entity_host, "127.0.0.1",
						   "min_bytes",
						   alert_threshold_exceeded,
						   alert_level_warning,
						   "miao");
  */

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_interface) ntop_interface->lua(vm);
  return(CONST_LUA_OK);
}
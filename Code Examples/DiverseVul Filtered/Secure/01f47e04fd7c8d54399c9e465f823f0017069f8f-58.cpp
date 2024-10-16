static int ntop_interface_make_room_alerts(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  int alert_entity;
  char *alert_entity_value, *table_name;
  AlertsManager *am;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  alert_entity = (int)lua_tonumber(vm, 1);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  alert_entity_value = (char*)lua_tostring(vm, 2);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING)) return(CONST_LUA_ERROR);
  table_name = (char*)lua_tostring(vm, 3);

  if((!ntop_interface)
     || ((am = ntop_interface->getAlertsManager()) == NULL))
    return(CONST_LUA_ERROR);

  am->makeRoom((AlertEntity)alert_entity, alert_entity_value, table_name);

  return CONST_LUA_OK;
}
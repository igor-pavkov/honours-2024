static int ntop_interface_query_alerts_raw(lua_State* vm) {
  NetworkInterface *iface = getCurrentInterface(vm);
  AlertsManager *am;
  bool engaged = false;
  char *selection = NULL, *clauses = NULL;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!iface || !(am = iface->getAlertsManager()))
    return (CONST_LUA_ERROR);

  if(lua_type(vm, 1) == LUA_TBOOLEAN)
    engaged = lua_toboolean(vm, 1);

  if(lua_type(vm, 2) == LUA_TSTRING)
    if((selection = (char*)lua_tostring(vm, 2)) == NULL)
      return(CONST_LUA_PARAM_ERROR);

  if(lua_type(vm, 3) == LUA_TSTRING)
    if((clauses = (char*)lua_tostring(vm, 3)) == NULL)
      return(CONST_LUA_PARAM_ERROR);

  if(am->queryAlertsRaw(vm, engaged, selection, clauses))
    return(CONST_LUA_ERROR);

  return (CONST_LUA_OK);
}
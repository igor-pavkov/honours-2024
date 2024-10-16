static int ntop_interface_make_room_requested(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  AlertsManager *am;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if((!ntop_interface)
     || ((am = ntop_interface->getAlertsManager()) == NULL))
    return(CONST_LUA_ERROR);

  lua_pushboolean(vm, am->makeRoomRequested());

  return(CONST_LUA_OK);

}
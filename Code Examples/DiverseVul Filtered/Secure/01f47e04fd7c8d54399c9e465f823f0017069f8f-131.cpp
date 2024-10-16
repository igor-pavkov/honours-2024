static int ntop_get_interface_host_activity(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  const char * host = NULL;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if (lua_type(vm, 1) == LUA_TSTRING)
    host = lua_tostring(vm, 1);

  if (ntop_interface == NULL || host == NULL)
    return CONST_LUA_ERROR;

  ntop_interface->getLocalHostActivity(vm, host);

  return CONST_LUA_OK;
}
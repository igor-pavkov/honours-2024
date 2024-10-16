static int ntop_get_interface_endpoint(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  u_int8_t id;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) != LUA_TNUMBER) /* Optional */
    id = 0;
  else
    id = (u_int8_t)lua_tonumber(vm, 1);

  if(ntop_interface) {
    char *endpoint = ntop_interface->getEndpoint(id); /* CHECK */

    lua_pushfstring(vm, "%s", endpoint ? endpoint : "");
  }

  return(CONST_LUA_OK);
}
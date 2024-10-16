static int ntop_get_ndpi_protocols(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  ndpi_protocol_category_t category_filter;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if((lua_type(vm, 1) == LUA_TNUMBER)) {
    category_filter = (ndpi_protocol_category_t)lua_tointeger(vm, 1);

    if(category_filter >= NDPI_PROTOCOL_NUM_CATEGORIES)
      return (CONST_LUA_ERROR);

    ntop_interface->getnDPIProtocols(vm, category_filter);
  } else
    ntop_interface->getnDPIProtocols(vm);

  return(CONST_LUA_OK);
}
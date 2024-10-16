static int ntop_interface_is_pcap_dump_interface(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  const char *interface_type;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface
     || ((interface_type = ntop_interface->get_type()) == NULL))
    return(CONST_LUA_ERROR);

  lua_pushboolean(vm, strcmp(interface_type, CONST_INTERFACE_TYPE_PCAP_DUMP) == 0);
  return(CONST_LUA_OK);
}
static int ntop_get_interface_names(lua_State* vm) {
  lua_newtable(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  for(int i=0; i<ntop->get_num_interfaces(); i++) {
    NetworkInterface *iface;

    if((iface = ntop->getInterfaceAtId(vm, i)) != NULL) {
      char num[8];

      ntop->getTrace()->traceEvent(TRACE_DEBUG, "Returning name %s", iface->get_name());
      snprintf(num, sizeof(num), "%d", i);
      lua_push_str_table_entry(vm, num, iface->get_name());
    }
  }

  return(CONST_LUA_OK);
}
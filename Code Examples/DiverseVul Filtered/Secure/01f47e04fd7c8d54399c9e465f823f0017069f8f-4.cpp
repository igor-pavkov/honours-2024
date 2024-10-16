static NetworkInterface* handle_null_interface(lua_State* vm) {
  char allowed_ifname[MAX_INTERFACE_NAME_LEN];

  ntop->getTrace()->traceEvent(TRACE_INFO, "NULL interface: did you restart ntopng in the meantime?");

  if(ntop->getInterfaceAllowed(vm, allowed_ifname)) {
      return ntop->getNetworkInterface(allowed_ifname);
  }

  return(ntop->getInterfaceAtId(0));
}
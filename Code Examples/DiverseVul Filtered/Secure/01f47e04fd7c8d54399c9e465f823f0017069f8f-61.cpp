static int ntop_delete_dump_files(lua_State *vm) {
  int ifid;
  char pcap_path[MAX_PATH];
  NetworkInterface *iface;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  if((ifid = lua_tointeger(vm, 1)) < 0) return(CONST_LUA_ERROR);
  if(!(iface = ntop->getNetworkInterface(ifid))) return(CONST_LUA_ERROR);

  snprintf(pcap_path, sizeof(pcap_path), "%s/%d/pcap/",
	   ntop->get_working_dir(), ifid);
  ntop->fixPath(pcap_path);

  if(Utils::discardOldFilesExceeding(pcap_path, iface->getDumpTrafficMaxFiles()))
    return(CONST_LUA_ERROR);

  return(CONST_LUA_OK);
}
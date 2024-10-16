static int ntop_get_interface_flows(lua_State* vm, LocationPolicy location) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char buf[64];
  char *host_ip = NULL;
  u_int16_t vlan_id = 0;
  Host *host = NULL;
  Paginator *p = NULL;
  int numFlows = -1;

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  if((p = new(std::nothrow) Paginator()) == NULL)
    return(CONST_LUA_ERROR);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TSTRING) {
    get_host_vlan_info((char*)lua_tostring(vm, 1), &host_ip, &vlan_id, buf, sizeof(buf));
    host = ntop_interface->getHost(host_ip, vlan_id);
  }

  if(lua_type(vm, 2) == LUA_TTABLE)
    p->readOptions(vm, 2);

  if(ntop_interface)
    numFlows = ntop_interface->getFlows(vm, get_allowed_nets(vm), location, host, p);

  if(p) delete p;
  return numFlows < 0 ? CONST_LUA_ERROR : CONST_LUA_OK;
}
static int ntop_get_interface_host_activitymap(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *host_ip;
  GenericHost *h;
  u_int16_t vlan_id = 0;
  char buf[64];

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)  return(CONST_LUA_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  get_host_vlan_info((char*)lua_tostring(vm, 1), &host_ip, &vlan_id, buf, sizeof(buf));

  /* Optional VLAN id */
  if(lua_type(vm, 2) == LUA_TNUMBER) vlan_id = (u_int16_t)lua_tonumber(vm, 2);

  h = ntop_interface->getHost(host_ip, vlan_id);

  if(h == NULL)
    return(CONST_LUA_ERROR);
  else {
    if(h->match(get_allowed_nets(vm))) {
      char *json = h->getJsonActivityMap();

      lua_pushfstring(vm, "%s", json);
      free(json);
    }

    return(CONST_LUA_OK);
  }
}
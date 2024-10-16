static int ntop_get_host_hit_rate(lua_State* vm) {
#ifdef NOTUSED
    NetworkInterface *ntop_interface = getCurrentInterface(vm);
  char *host_ip;
  u_int16_t vlan_id = 0;
  char buf[64];
  Host *h;
  u_int32_t peer_key;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  peer_key = (u_int32_t)lua_tonumber(vm, 1);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_ERROR);
  get_host_vlan_info((char*)lua_tostring(vm, 2), &host_ip, &vlan_id, buf, sizeof(buf));

  /* Optional VLAN id */
  if(lua_type(vm, 3) == LUA_TNUMBER) vlan_id = (u_int16_t)lua_tonumber(vm, 3);

  if((!ntop_interface)
     || ((h = ntop_interface->findHostsByIP(get_allowed_nets(vm), host_ip, vlan_id)) == NULL))
    return(CONST_LUA_ERROR);

  h->getPeerBytes(vm, peer_key);
  return(CONST_LUA_OK);
#else
  return(CONST_LUA_ERROR); // not supported
#endif
}
static int ntop_get_interface_flow_key(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  Host *cli, *srv;
  char *cli_name = NULL; u_int16_t cli_vlan = 0; u_int16_t cli_port = 0;
  char *srv_name = NULL; u_int16_t srv_vlan = 0; u_int16_t srv_port = 0;
  u_int16_t protocol;
  char cli_buf[256], srv_buf[256];

  if(!ntop_interface)
    return(CONST_LUA_ERROR);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)    /* cli_host@cli_vlan */
     || ntop_lua_check(vm, __FUNCTION__, 2, LUA_TNUMBER) /* cli port          */
     || ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING) /* srv_host@srv_vlan */
     || ntop_lua_check(vm, __FUNCTION__, 4, LUA_TNUMBER) /* srv port          */
     || ntop_lua_check(vm, __FUNCTION__, 5, LUA_TNUMBER) /* protocol          */
     ) return(CONST_LUA_ERROR);

  get_host_vlan_info((char*)lua_tostring(vm, 1), &cli_name, &cli_vlan, cli_buf, sizeof(cli_buf));
  cli_port = htons((u_int16_t)lua_tonumber(vm, 2));

  get_host_vlan_info((char*)lua_tostring(vm, 3), &srv_name, &srv_vlan, srv_buf, sizeof(srv_buf));
  srv_port = htons((u_int16_t)lua_tonumber(vm, 4));

  protocol = (u_int16_t)lua_tonumber(vm, 5);

  if(cli_vlan != srv_vlan) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "Client and Server vlans don't match.");
    return(CONST_LUA_ERROR);
  }

  if(cli_name == NULL || srv_name == NULL
     ||(cli = ntop_interface->getHost(cli_name, cli_vlan)) == NULL
     ||(srv = ntop_interface->getHost(srv_name, srv_vlan)) == NULL) {
    lua_pushnil(vm);
  } else {
    lua_pushnumber(vm, Flow::key(cli, cli_port, srv, srv_port, cli_vlan, protocol));
  }

  return(CONST_LUA_OK);
}
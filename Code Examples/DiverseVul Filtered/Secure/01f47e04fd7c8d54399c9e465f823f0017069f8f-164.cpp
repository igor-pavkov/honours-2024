static int ntop_snmp_get_fctn(lua_State* vm, int operation) {
  char *agent_host, *oid, *community;
  u_int agent_port = 161, timeout = 5, request_id = (u_int)time(NULL);
  int sock, i = 0, rc = CONST_LUA_OK;
  SNMPMessage *message;
  int len;
  unsigned char *buf;
  bool debug = false;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING))  return(CONST_LUA_ERROR);
  agent_host = (char*)lua_tostring(vm, 1);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING))  return(CONST_LUA_ERROR);
  community = (char*)lua_tostring(vm, 2);

  if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING))  return(CONST_LUA_ERROR);
  oid = (char*)lua_tostring(vm, 3);

  sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

  if(sock < 0) return(CONST_LUA_ERROR);

  message = snmp_create_message();
  snmp_set_version(message, 0);
  snmp_set_community(message, community);
  snmp_set_pdu_type(message, operation);
  snmp_set_request_id(message, request_id);
  snmp_set_error(message, 0);
  snmp_set_error_index(message, 0);
  snmp_add_varbind_null(message, oid);

  /* Add additional OIDs */
  i = 4;
  while(lua_type(vm, i) == LUA_TSTRING) {
    snmp_add_varbind_null(message, (char*)lua_tostring(vm, i));
    i++;
  }

  len = snmp_message_length(message);
  buf = (unsigned char*)malloc(len);
  snmp_render_message(message, buf);
  snmp_destroy_message(message);

  send_udp_datagram(buf, len, sock, agent_host, agent_port);
  free(buf);

  if(debug)
    ntop->getTrace()->traceEvent(TRACE_NORMAL, "SNMP %s %s@%s %s",
				 (operation == SNMP_GET_REQUEST_TYPE) ? "Get" : "GetNext",
				 agent_host, community, oid);

  if(input_timeout(sock, timeout) == 0) {
    /* Timeout */

    if(debug)
      ntop->getTrace()->traceEvent(TRACE_NORMAL, "SNMP Timeout %s@%s %s", agent_host, community, oid);
    rc = CONST_LUA_ERROR;
    lua_pushnil(vm);
  } else {
    char buf[BUFLEN];
    SNMPMessage *message;
    char *sender_host, *oid_str,  *value_str;
    int sender_port, added = 0, len;

    len = receive_udp_datagram(buf, BUFLEN, sock, &sender_host, &sender_port);
    message = snmp_parse_message(buf, len);

    i = 0;
    while(snmp_get_varbind_as_string(message, i, &oid_str, NULL, &value_str)) {
      if(!added) lua_newtable(vm), added = 1;
      lua_push_str_table_entry(vm, oid_str, value_str);
      if(debug)
	ntop->getTrace()->traceEvent(TRACE_NORMAL, "SNMP OK %s@%s %s=%s", agent_host, community, oid_str, value_str);
      i++;
    }

    snmp_destroy_message(message);

    if(!added) {
      ntop->getTrace()->traceEvent(TRACE_ERROR, "SNMP Error %s@%s", agent_host, community);
      lua_pushnil(vm), rc = CONST_LUA_ERROR;
    }
  }

  closesocket(sock);
  return(rc);
}
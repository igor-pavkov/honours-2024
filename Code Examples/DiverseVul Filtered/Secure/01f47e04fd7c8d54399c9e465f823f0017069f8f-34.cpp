static int ntop_get_ndpi_protocol_category(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  u_int proto;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  proto = (u_int)lua_tonumber(vm, 1);

  if(ntop_interface) {
    ndpi_protocol_category_t category = ntop_interface->get_ndpi_proto_category(proto);

    lua_newtable(vm);
    lua_push_int32_table_entry(vm, "id", category);
    lua_push_str_table_entry(vm, "name", (char*)ndpi_category_str(category));
  } else
    lua_pushnil(vm);

  return(CONST_LUA_OK);
}
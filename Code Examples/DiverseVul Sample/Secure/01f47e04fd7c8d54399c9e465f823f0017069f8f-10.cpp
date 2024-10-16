static int ntop_get_interface_find_flow_by_key(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  u_int32_t key;
  Flow *f;
  AddressTree *ptree = get_allowed_nets(vm);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TNUMBER)) return(CONST_LUA_ERROR);
  key = (u_int32_t)lua_tonumber(vm, 1);

  if(!ntop_interface) return(false);

  f = ntop_interface->findFlowByKey(key, ptree);

  if(f == NULL)
    return(CONST_LUA_ERROR);
  else {
    f->lua(vm, ptree, details_high, false);
    return(CONST_LUA_OK);
  }
}
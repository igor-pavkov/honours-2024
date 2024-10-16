static AddressTree* get_allowed_nets(lua_State* vm) {
  AddressTree *ptree;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_getglobal(vm, CONST_ALLOWED_NETS);
  ptree = (AddressTree*)lua_touserdata(vm, lua_gettop(vm));
  //ntop->getTrace()->traceEvent(TRACE_WARNING, "GET %p", ptree);
  return(ptree);
}
static int ntop_inet_ntoa(lua_State* vm) {
  u_int32_t ip;
  struct in_addr in;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TSTRING)
    ip = atol((char*)lua_tostring(vm, 1));
  else if(lua_type(vm, 1) == LUA_TNUMBER)
    ip = (u_int32_t)lua_tonumber(vm, 1);
  else
    return(CONST_LUA_ERROR);

  in.s_addr = htonl(ip);
  lua_pushstring(vm, inet_ntoa(in));
  return(CONST_LUA_OK);
}
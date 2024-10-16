static int ntop_get_resolved_address(lua_State* vm) {
  char *key, *tmp,rsp[256],value[64];
  Redis *redis = ntop->getRedis();
  u_int16_t vlan_id = 0;
  char buf[64];

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  get_host_vlan_info((char*)lua_tostring(vm, 1), &key, &vlan_id, buf, sizeof(buf));

  if(key == NULL)
    return(CONST_LUA_ERROR);

  if(redis->getAddress(key, rsp, sizeof(rsp), true) == 0)
    tmp = rsp;
  else
    tmp = key;

  if(vlan_id != 0)
    snprintf(value, sizeof(value), "%s@%u", tmp, vlan_id);
  else
    snprintf(value, sizeof(value), "%s", tmp);

#if 0
  if(!strcmp(value, key)) {
    char rsp[64];

    if((ntop->getRedis()->hashGet((char*)HOST_LABEL_NAMES, key, rsp, sizeof(rsp)) == 0)
       && (rsp[0] !='\0'))
      lua_pushfstring(vm, "%s", rsp);
    else
      lua_pushfstring(vm, "%s", value);
  } else
    lua_pushfstring(vm, "%s", value);
#else
  lua_pushfstring(vm, "%s", value);
#endif

  return(CONST_LUA_OK);
}
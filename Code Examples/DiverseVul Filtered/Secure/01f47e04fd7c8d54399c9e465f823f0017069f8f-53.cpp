static int ntop_lrange_redis(lua_State* vm) {
  char *l_name, **l_elements;
  Redis *redis = ntop->getRedis();
  int start_offset = 0, end_offset = -1;
  int rc;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((l_name = (char*)lua_tostring(vm, 1)) == NULL)   return(CONST_LUA_PARAM_ERROR);

  if(lua_type(vm, 2) == LUA_TNUMBER) {
    start_offset = lua_tointeger(vm, 2);
  }
  if(lua_type(vm, 3) == LUA_TNUMBER) {
    end_offset = lua_tointeger(vm, 3);
  }

  rc = redis->lrange(l_name, &l_elements, start_offset, end_offset);

  if(rc > 0) {
    lua_newtable(vm);

    for(int i = 0; i < rc; i++) {
      lua_push_str_table_entry(vm, l_elements[i] ? l_elements[i] : "", (char*)"");
      if(l_elements[i]) free(l_elements[i]);
    }
    free(l_elements);
  } else
    lua_pushnil(vm);

  return(CONST_LUA_OK);
}
static int ntop_get_hash_keys_redis(lua_State* vm) {
  char *key, **vals;
  Redis *redis = ntop->getRedis();
  int rc;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((key = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);

  rc = redis->hashKeys(key, &vals);

  if(rc > 0) {
    lua_newtable(vm);

    for(int i = 0; i < rc; i++) {
      lua_push_str_table_entry(vm, vals[i] ? vals[i] : "", (char*)"");
      if(vals[i]) free(vals[i]);
    }
    free(vals);
  } else
    lua_pushnil(vm);

  return(CONST_LUA_OK);
}
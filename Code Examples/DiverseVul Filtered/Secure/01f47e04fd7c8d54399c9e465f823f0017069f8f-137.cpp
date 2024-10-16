static int ntop_rrd_update(lua_State* vm) {
  const char *filename, *update_arg;
  int status;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((filename = (const char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((update_arg = (const char*)lua_tostring(vm, 2)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s(%s) %s", __FUNCTION__, filename, update_arg);

  reset_rrd_state();
  status = rrd_update_r(filename, NULL, 1, &update_arg);

  if(status != 0) {
    char *err = rrd_get_error();

    if(err != NULL) {
      luaL_error(vm, err);
      return(CONST_LUA_ERROR);
    }
  }

  return(CONST_LUA_OK);
}
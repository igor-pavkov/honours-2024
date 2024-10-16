static int __ntop_rrd_args (lua_State* vm, char **filename, char **cf, time_t *start, time_t *end) {
  char *start_s, *end_s, *err;
  rrd_time_value_t start_tv, end_tv;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((*filename = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((*cf = (char*)lua_tostring(vm, 2)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if((lua_type(vm, 3) == LUA_TNUMBER) && (lua_type(vm, 4) == LUA_TNUMBER))
    *start = (time_t)lua_tonumber(vm, 3), *end = (time_t)lua_tonumber(vm, 4);
  else {
    if(ntop_lua_check(vm, __FUNCTION__, 3, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
    if((start_s = (char*)lua_tostring(vm, 3)) == NULL)  return(CONST_LUA_PARAM_ERROR);

    if((err = rrd_parsetime(start_s, &start_tv)) != NULL) {
      luaL_error(vm, err);
      return(CONST_LUA_PARAM_ERROR);
    }

    if(ntop_lua_check(vm, __FUNCTION__, 4, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
    if((end_s = (char*)lua_tostring(vm, 4)) == NULL)  return(CONST_LUA_PARAM_ERROR);

    if((err = rrd_parsetime(end_s, &end_tv)) != NULL) {
      luaL_error(vm, err);
      return(CONST_LUA_PARAM_ERROR);
    }

    if(rrd_proc_start_end(&start_tv, &end_tv, start, end) == -1)
      return(CONST_LUA_PARAM_ERROR);
  }

  return(CONST_LUA_OK);
}
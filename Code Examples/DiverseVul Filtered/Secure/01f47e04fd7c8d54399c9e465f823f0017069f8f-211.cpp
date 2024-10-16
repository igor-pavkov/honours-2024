static int ntop_rrd_lastupdate(lua_State* vm) {
  const char *filename;
  time_t    last_update;
  char    **ds_names;
  char    **last_ds;
  unsigned long ds_count, i;
  int status;

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((filename = (const char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  status = rrd_lastupdate_r(filename, &last_update, &ds_count, &ds_names, &last_ds);

  if(status != 0) {
    return(CONST_LUA_ERROR);
  } else {
    for(i = 0; i < ds_count; i++)
      free(last_ds[i]), free(ds_names[i]);

    free(last_ds), free(ds_names);

    lua_pushnumber(vm, last_update);
    lua_pushnumber(vm, ds_count);
    return(2 /* 2 values returned */);
  }
}
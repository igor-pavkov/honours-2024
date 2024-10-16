static int ntop_rrd_fetch_columns(lua_State* vm) {
  char *filename, *cf;
  time_t start, end;
  int status;
  unsigned int npoints = 0, i, j;
  char **names;
  unsigned long step = 0, ds_cnt = 0;
  rrd_value_t *data, *p;

  if ((status = __ntop_rrd_args(vm, &filename, &cf, &start, &end)) != CONST_LUA_OK) return status;

  ntop->getTrace()->traceEvent(TRACE_INFO, "%s(%s)", __FUNCTION__, filename);

  reset_rrd_state();

  if ((status = __ntop_rrd_status(vm, rrd_fetch_r(filename, cf, &start, &end, &step, &ds_cnt, &names, &data), filename, cf)) != CONST_LUA_OK) return status;

  npoints = (end - start) / step;

  lua_pushnumber(vm, (lua_Number) start);
  lua_pushnumber(vm, (lua_Number) step);

  /* create the data series table */
  lua_createtable(vm, 0, ds_cnt);

  for(i=0; i<ds_cnt; i++) {
    /* a single serie table, preallocated */
    lua_createtable(vm, npoints, 0);
    p = data + i;

    for(j=0; j<npoints; j++) {
      rrd_value_t value = *p;
      /* we are accessing data table by columns */
      p = p + ds_cnt;
      lua_pushnumber(vm, (lua_Number)value);
      lua_rawseti(vm, -2, j+1);
    }

    /* add the single serie to the series table */
    lua_setfield(vm, -2, names[i]);
    rrd_freemem(names[i]);
  }

  rrd_freemem(names);
  rrd_freemem(data);

  /* end and npoints as last values */
  lua_pushnumber(vm, (lua_Number) end);
  lua_pushnumber(vm, (lua_Number) npoints);

  /* number of return values */
  return(5);
}
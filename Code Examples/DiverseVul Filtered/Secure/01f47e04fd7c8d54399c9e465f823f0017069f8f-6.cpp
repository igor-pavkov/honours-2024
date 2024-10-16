static int __ntop_rrd_status(lua_State* vm, int status, char *filename, char *cf) {
  char * err;

  if(status != 0) {
    err = rrd_get_error();

    if(err != NULL) {
      ntop->getTrace()->traceEvent(TRACE_ERROR,
                                   "Error '%s' while calling rrd_fetch_r(%s, %s): is the RRD corrupted perhaps?",
                                   err, filename, cf);
      lua_pushnil(vm);
      lua_pushnil(vm);
      lua_pushnil(vm);
      lua_pushnil(vm);
      return(CONST_LUA_ERROR);
    }
  }

  return(CONST_LUA_OK);
}
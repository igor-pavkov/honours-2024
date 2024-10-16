static int ntop_dump_file(lua_State* vm) {
  char *fname;
  FILE *fd;
  struct mg_connection *conn;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_getglobal(vm, CONST_HTTP_CONN);
  if((conn = (struct mg_connection*)lua_touserdata(vm, lua_gettop(vm))) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "INTERNAL ERROR: null HTTP connection");
    return(CONST_LUA_ERROR);
  }

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((fname = (char*)lua_tostring(vm, 1)) == NULL)     return(CONST_LUA_PARAM_ERROR);

  ntop->fixPath(fname);
  if((fd = fopen(fname, "r")) != NULL) {
    char tmp[1024];

    ntop->getTrace()->traceEvent(TRACE_INFO, "[HTTP] Serving file %s", fname);

    while((fgets(tmp, sizeof(tmp)-256 /* To make sure we have room for replacements */, fd)) != NULL) {
      for(int i=0; string_to_replace[i].key != NULL; i++)
	Utils::replacestr(tmp, string_to_replace[i].key, string_to_replace[i].val);

      mg_printf(conn, "%s", tmp);
    }

    fclose(fd);
    return(CONST_LUA_OK);
  } else {
    ntop->getTrace()->traceEvent(TRACE_INFO, "Unable to read file %s", fname);
    return(CONST_LUA_ERROR);
  }
}
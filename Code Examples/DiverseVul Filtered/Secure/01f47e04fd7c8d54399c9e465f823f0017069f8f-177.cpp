static int ntop_lua_http_print(lua_State* vm) {
  struct mg_connection *conn;
  char *printtype;
  int t;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_getglobal(vm, CONST_HTTP_CONN);
  if((conn = (struct mg_connection*)lua_touserdata(vm, lua_gettop(vm))) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "INTERNAL ERROR: null HTTP connection");
    return(CONST_LUA_OK);
  }

  /* Handle binary blob */
  if(lua_type(vm, 2) == LUA_TSTRING &&
     (printtype = (char*)lua_tostring(vm, 2)) != NULL)
    if(!strncmp(printtype, "blob", 4)) {
      char *str = NULL;

      if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return (CONST_LUA_ERROR);
      if((str = (char*)lua_tostring(vm, 1)) != NULL) {
	int len = strlen(str);

	if(len <= 1)
	  mg_printf(conn, "%c", str[0]);
	else
	  return (CONST_LUA_PARAM_ERROR);
      }

      return (CONST_LUA_OK);
    }

  switch(t = lua_type(vm, 1)) {
  case LUA_TNIL:
    mg_printf(conn, "%s", "nil");
    break;

  case LUA_TBOOLEAN:
    {
      int v = lua_toboolean(vm, 1);

      mg_printf(conn, "%s", v ? "true" : "false");
    }
    break;

  case LUA_TSTRING:
    {
      char *str = (char*)lua_tostring(vm, 1);

      if(str && (strlen(str) > 0))
	mg_printf(conn, "%s", str);
    }
    break;

  case LUA_TNUMBER:
    {
      char str[64];

      snprintf(str, sizeof(str), "%f", (float)lua_tonumber(vm, 1));
      mg_printf(conn, "%s", str);
    }
    break;

  default:
    ntop->getTrace()->traceEvent(TRACE_WARNING, "%s(): Lua type %d is not handled",
				 __FUNCTION__, t);
    return(CONST_LUA_ERROR);
  }

  return(CONST_LUA_OK);
}
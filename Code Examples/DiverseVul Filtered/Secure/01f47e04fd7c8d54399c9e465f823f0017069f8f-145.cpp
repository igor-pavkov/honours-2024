static int ntop_interface_exec_sql_query(lua_State *vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  bool limit_rows = true;  // honour the limit by default
  bool wait_for_db_created = true;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(!ntop_interface)
    return(CONST_LUA_ERROR);
  else {
    char *sql;

    if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
    if((sql = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

    if(lua_type(vm, 2) == LUA_TBOOLEAN) {
      limit_rows = lua_toboolean(vm, 2) ? true : false;
    }

    if(lua_type(vm, 3) == LUA_TBOOLEAN) {
      wait_for_db_created = lua_toboolean(vm, 3) ? true : false;
    }

    if(ntop_interface->exec_sql_query(vm, sql, limit_rows, wait_for_db_created) < 0)
      lua_pushnil(vm);

    return(CONST_LUA_OK);
  }
}
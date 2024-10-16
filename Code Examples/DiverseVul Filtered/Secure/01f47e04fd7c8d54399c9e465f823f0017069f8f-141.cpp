static int ntop_sqlite_exec_query(lua_State* vm) {
  char *db_path, *db_query;
  sqlite3 *db;
  char *zErrMsg = 0;
  struct ntopng_sqlite_state state;
  struct stat buf;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING))  return(CONST_LUA_ERROR);
  db_path = (char*)lua_tostring(vm, 1);

  if(ntop_lua_check(vm, __FUNCTION__, 2, LUA_TSTRING))  return(CONST_LUA_ERROR);
  db_query = (char*)lua_tostring(vm, 2);

  if(stat(db_path, &buf) != 0) {
    ntop->getTrace()->traceEvent(TRACE_INFO, "Not found database %s",
				 db_path);
    return(CONST_LUA_ERROR);
  }

  if(sqlite3_open(db_path, &db)) {
    ntop->getTrace()->traceEvent(TRACE_INFO, "Unable to open %s: %s",
				 db_path, sqlite3_errmsg(db));
    return(CONST_LUA_ERROR);
  }

  state.vm = vm, state.num_rows = 0;
  lua_newtable(vm);
  if(sqlite3_exec(db, db_query, sqlite_callback, (void*)&state, &zErrMsg)) {
    ntop->getTrace()->traceEvent(TRACE_INFO, "SQL Error: %s", zErrMsg);
    sqlite3_free(zErrMsg);
  }

  sqlite3_close(db);
  return(CONST_LUA_OK);
}
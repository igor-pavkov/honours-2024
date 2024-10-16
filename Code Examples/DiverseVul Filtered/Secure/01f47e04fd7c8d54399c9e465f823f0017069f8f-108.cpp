static int ntop_remove_dir_recursively(lua_State* vm) {
  char *path;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  path = (char*)lua_tostring(vm, 1);
  ntop->fixPath(path);

  remove_recursively(path);

  return(CONST_LUA_OK);
}
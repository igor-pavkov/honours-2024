static int ntop_get_file_last_change(lua_State* vm) {
  char *path;
  struct stat buf;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  path = (char*)lua_tostring(vm, 1);

  if(stat(path, &buf) == 0)
    lua_pushnumber(vm, (lua_Number)buf.st_mtime);
  else
    lua_pushnumber(vm, -1); /* not found */

  return(CONST_LUA_OK);
}
static int ntop_is_dir(lua_State* vm) {
  char *path;
  struct stat buf;
  int rc;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  path = (char*)lua_tostring(vm, 1);

  rc = ((stat(path, &buf) != 0) || (!S_ISDIR(buf.st_mode))) ? 0 : 1;
  lua_pushboolean(vm, rc);

  return(CONST_LUA_OK);
}
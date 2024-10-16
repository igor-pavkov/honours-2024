static int ntop_mkdir_tree(lua_State* vm) {
  char *dir;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  if((dir = (char*)lua_tostring(vm, 1)) == NULL)       return(CONST_LUA_PARAM_ERROR);
  if(dir[0] == '\0')                                   return(CONST_LUA_OK); /* Nothing to do */

  return(Utils::mkdir_tree(dir));
}
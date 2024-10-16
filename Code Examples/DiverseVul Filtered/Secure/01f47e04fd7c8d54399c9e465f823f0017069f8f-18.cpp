static int ntop_http_get(lua_State* vm) {
  char *url, *username = NULL, *pwd = NULL;
  int timeout = 30;
  bool return_content = true;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_PARAM_ERROR);
  if((url = (char*)lua_tostring(vm, 1)) == NULL)  return(CONST_LUA_PARAM_ERROR);

  if(lua_type(vm, 2) == LUA_TSTRING) {
    username = (char*)lua_tostring(vm, 2);

    if(lua_type(vm, 3) == LUA_TSTRING) {
      pwd = (char*)lua_tostring(vm, 3);

      if(lua_type(vm, 4) == LUA_TNUMBER) {
	timeout = lua_tointeger(vm, 4);
	if(timeout < 1) timeout = 1;

	/*
	  This optional parameter specifies if the result of HTTP GET has to be returned
	  to LUA or not. Usually the content has to be returned, but in some causes
	  it just matters to time (for instance when use for testing HTTP services)
	*/
	if(lua_type(vm, 4) == LUA_TBOOLEAN) {
	  return_content = lua_toboolean(vm, 5) ? true : false;
	}
      }
    }
  }

  if(Utils::httpGet(vm, url, username, pwd, timeout, return_content))
    return(CONST_LUA_OK);
  else
    return(CONST_LUA_ERROR);
}
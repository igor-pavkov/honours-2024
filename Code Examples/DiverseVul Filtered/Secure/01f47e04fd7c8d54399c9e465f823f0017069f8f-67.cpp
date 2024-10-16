static int ntop_list_dir_files(lua_State* vm) {
  char *path;
  DIR *dirp;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(ntop_lua_check(vm, __FUNCTION__, 1, LUA_TSTRING)) return(CONST_LUA_ERROR);
  path = (char*)lua_tostring(vm, 1);
  ntop->fixPath(path);

  lua_newtable(vm);

  if((dirp = opendir(path)) != NULL) {
    struct dirent *dp;

    while ((dp = readdir(dirp)) != NULL)
      if((dp->d_name[0] != '\0')
	 && (dp->d_name[0] != '.')) {
	lua_push_str_table_entry(vm, dp->d_name, dp->d_name);
      }
    (void)closedir(dirp);
  }

  return(CONST_LUA_OK);
}
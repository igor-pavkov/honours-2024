static int ntop_list_reports(lua_State* vm) {
  DIR *dir;
  char fullpath[MAX_PATH];

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_newtable(vm);
  snprintf(fullpath, sizeof(fullpath), "%s/%s", ntop->get_working_dir(), "reports");
  ntop->fixPath(fullpath);
  if((dir = opendir(fullpath)) != NULL) {
    struct dirent *ent;

    while ((ent = readdir(dir)) != NULL) {
      char filepath[MAX_PATH];
      snprintf(filepath, sizeof(filepath), "%s/%s", fullpath, ent->d_name);
      ntop->fixPath(filepath);
      struct stat buf;
      if(!stat(filepath, &buf) && !S_ISDIR(buf.st_mode))
	lua_push_str_table_entry(vm, ent->d_name, (char*)"");
    }
    closedir(dir);
  }
  return(CONST_LUA_OK);
}
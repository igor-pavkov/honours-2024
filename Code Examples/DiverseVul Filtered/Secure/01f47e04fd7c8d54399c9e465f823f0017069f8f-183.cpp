static int ntop_get_dirs(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_newtable(vm);
  lua_push_str_table_entry(vm, "installdir", ntop->get_install_dir());
  lua_push_str_table_entry(vm, "workingdir", ntop->get_working_dir());
  lua_push_str_table_entry(vm, "scriptdir", ntop->getPrefs()->get_scripts_dir());
  lua_push_str_table_entry(vm, "httpdocsdir", ntop->getPrefs()->get_docs_dir());
  lua_push_str_table_entry(vm, "callbacksdir", ntop->getPrefs()->get_callbacks_dir());

  return(CONST_LUA_OK);
}
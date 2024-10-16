static int ntop_reload_preferences(lua_State* vm) {
  lua_newtable(vm);
  ntop->getPrefs()->reloadPrefsFromRedis();

  return(CONST_LUA_OK);
}
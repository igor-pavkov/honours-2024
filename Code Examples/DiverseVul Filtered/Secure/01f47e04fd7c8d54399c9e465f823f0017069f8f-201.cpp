int Lua::run_script(char *script_path) {
  int rc = 0;

  if(!L) return(-1);

  try {
    luaL_openlibs(L); /* Load base libraries */
    lua_register_classes(L, false); /* Load custom classes */

#ifndef NTOPNG_PRO
    rc = luaL_dofile(L, script_path);
#else
    if(ntop->getPro()->has_valid_license())
      rc = __ntop_lua_handlefile(L, script_path, true);
    else
      rc = luaL_dofile(L, script_path);
#endif

    if(rc != 0) {
      const char *err = lua_tostring(L, -1);

      ntop->getTrace()->traceEvent(TRACE_WARNING, "Script failure [%s][%s]", script_path, err);
      rc = -1;
    }
  } catch(...) {
    ntop->getTrace()->traceEvent(TRACE_WARNING, "Script failure [%s]", script_path);
    rc = -2;
  }

  return(rc);
}
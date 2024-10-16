static int ntop_get_info(lua_State* vm) {
  char rsp[256];
  int major, minor, patch;
  bool verbose = true;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  if(lua_type(vm, 1) == LUA_TBOOLEAN)
    verbose = lua_toboolean(vm, 1) ? true : false;

  lua_newtable(vm);
  lua_push_str_table_entry(vm, "product", (char*)"ntopng");
  lua_push_str_table_entry(vm, "copyright", (char*)"&copy; 1998-17 - ntop.org");
  lua_push_str_table_entry(vm, "authors", (char*)"The ntop.org team");
  lua_push_str_table_entry(vm, "license", (char*)"GNU GPLv3");

  lua_push_str_table_entry(vm, "version", (char*)PACKAGE_VERSION);
  lua_push_str_table_entry(vm, "git", (char*)NTOPNG_GIT_RELEASE);

  snprintf(rsp, sizeof(rsp), "%s [%s][%s]",
	   PACKAGE_OSNAME, PACKAGE_MACHINE, PACKAGE_OS);
  lua_push_str_table_entry(vm, "platform", rsp);
  lua_push_str_table_entry(vm, "OS",
#ifdef WIN32
			   (char*)"Windows"
#else
			   (char*)PACKAGE_OS
#endif
			   );
  lua_push_int_table_entry(vm, "bits", (sizeof(void*) == 4) ? 32 : 64);
  lua_push_int_table_entry(vm, "uptime", ntop->getGlobals()->getUptime());
  lua_push_str_table_entry(vm, "command_line", ntop->getPrefs()->get_command_line());

  if(verbose) {
    lua_push_str_table_entry(vm, "version.rrd", rrd_strversion());
    lua_push_str_table_entry(vm, "version.redis", ntop->getRedis()->getVersion(rsp, sizeof(rsp)));
    lua_push_str_table_entry(vm, "version.httpd", (char*)mg_version());
    lua_push_str_table_entry(vm, "version.git", (char*)NTOPNG_GIT_RELEASE);
    lua_push_str_table_entry(vm, "version.luajit", (char*)LUAJIT_VERSION);
#ifdef HAVE_GEOIP
    lua_push_str_table_entry(vm, "version.geoip", (char*)GeoIP_lib_version());
#endif
    lua_push_str_table_entry(vm, "version.ndpi", ndpi_revision());
    lua_push_bool_table_entry(vm, "version.enterprise_edition", ntop->getPrefs()->is_enterprise_edition());
    lua_push_bool_table_entry(vm, "version.embedded_edition", ntop->getPrefs()->is_embedded_edition());

    lua_push_bool_table_entry(vm, "pro.release", ntop->getPrefs()->is_pro_edition());
    lua_push_int_table_entry(vm, "pro.demo_ends_at", ntop->getPrefs()->pro_edition_demo_ends_at());
#ifdef NTOPNG_PRO
    lua_push_str_table_entry(vm, "pro.license", ntop->getPro()->get_license());
    lua_push_bool_table_entry(vm, "pro.use_redis_license", ntop->getPro()->use_redis_license());
    lua_push_str_table_entry(vm, "pro.systemid", ntop->getPro()->get_system_id());
#endif

#if 0
    ntop->getRedis()->get((char*)CONST_STR_NTOPNG_LICENSE, rsp, sizeof(rsp));
    lua_push_str_table_entry(vm, "ntopng.license", rsp);
#endif

    zmq_version(&major, &minor, &patch);
    snprintf(rsp, sizeof(rsp), "%d.%d.%d", major, minor, patch);
    lua_push_str_table_entry(vm, "version.zmq", rsp);
  }

  return(CONST_LUA_OK);
}
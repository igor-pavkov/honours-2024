static int ntop_generate_csrf_value(lua_State* vm) {
  char random_a[32], random_b[32], csrf[33], user[64] = { '\0' };
  Redis *redis = ntop->getRedis();
  struct mg_connection *conn;

  lua_getglobal(vm, CONST_HTTP_CONN);
  if((conn = (struct mg_connection*)lua_touserdata(vm, lua_gettop(vm))) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "INTERNAL ERROR: null HTTP connection");
    return(CONST_LUA_OK);
  }

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

#ifdef __OpenBSD__
  snprintf(random_a, sizeof(random_a), "%d", arc4random());
  snprintf(random_b, sizeof(random_b), "%lu", time(NULL)*arc4random());
#else
  snprintf(random_a, sizeof(random_a), "%d", rand());
  snprintf(random_b, sizeof(random_b), "%lu", time(NULL)*rand());
#endif

  mg_get_cookie(conn, "user", user, sizeof(user));
  mg_md5(csrf, random_a, random_b, NULL);

  redis->set(csrf, (char*)user, MAX_CSRF_DURATION);
  lua_pushfstring(vm, "%s", csrf);
  return(CONST_LUA_OK);
}
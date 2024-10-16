int Lua::handle_script_request(struct mg_connection *conn,
			       const struct mg_request_info *request_info,
			       char *script_path) {
  char buf[64], key[64], ifname[MAX_INTERFACE_NAME_LEN];
  char *_cookies, user[64] = { '\0' };
  AddressTree ptree;
  int rc;
  const char * content_type;

  if(!L) return(-1);

  luaL_openlibs(L); /* Load base libraries */
  lua_register_classes(L, true); /* Load custom classes */

  lua_pushlightuserdata(L, (char*)conn);
  lua_setglobal(L, CONST_HTTP_CONN);

  content_type = mg_get_header(conn, "Content-Type");

  /* Check for POST requests */
  if((strcmp(request_info->request_method, "POST") == 0) &&
      ((content_type != NULL) && (strstr(content_type, "application/x-www-form-urlencoded") == content_type))) {
    char post_data[1024] = { '\0' };
    char rsp[32];
    char csrf[64] = { '\0' };
    char user[64] = { '\0' };
    int post_data_len = mg_read(conn, post_data, sizeof(post_data));
    u_int8_t valid_csrf = 1;

    post_data[sizeof(post_data)-1] = '\0';

    /* CSRF is mandatory in POST request */
    mg_get_var(post_data, post_data_len, "csrf", csrf, sizeof(csrf));
    mg_get_cookie(conn, "user", user, sizeof(user));

    if((ntop->getRedis()->get(csrf, rsp, sizeof(rsp)) == -1)
       || (strcmp(rsp, user) != 0)) {
#if 0
      const char *msg = "The submitted form is expired. Please reload the page and try again. <p>[ <A HREF=/>Home</A> ]";

      ntop->getTrace()->traceEvent(TRACE_WARNING,
          "Invalid CSRF parameter specified [%s][%s][%s][%s]: page expired?",
          csrf, rsp, user, "csrf");

      return(send_error(conn, 500 /* Internal server error */,
          msg, PAGE_ERROR, script_path, msg));
#else
      valid_csrf = 0;
#endif
    } else {
      /* Invalidate csrf */
      ntop->getRedis()->del(csrf);
    }

    if(valid_csrf)
      setParamsTable(L, "_POST", post_data); /* CSRF is valid here, now fill the _POST table with POST parameters */
    else
      setParamsTable(L, "_POST", NULL /* Empty */);
  } else
    setParamsTable(L, "_POST", NULL /* Empty */);

  /* Put the GET params into the environment */
  if(request_info->query_string)
    setParamsTable(L, "_GET", request_info->query_string);
  else
    setParamsTable(L, "_GET", NULL /* Empty */);

  /* _SERVER */
  lua_newtable(L);
  lua_push_str_table_entry(L, "REQUEST_METHOD", (char*)request_info->request_method);
  lua_push_str_table_entry(L, "URI", (char*)request_info->uri ? (char*)request_info->uri : (char*)"");
  lua_push_str_table_entry(L, "REFERER", (char*)mg_get_header(conn, "Referer") ? (char*)mg_get_header(conn, "Referer") : (char*)"");
  if(request_info->remote_user)  lua_push_str_table_entry(L, "REMOTE_USER", (char*)request_info->remote_user);
  if(request_info->query_string) lua_push_str_table_entry(L, "QUERY_STRING", (char*)request_info->query_string);

  for(int i=0; ((request_info->http_headers[i].name != NULL) 
		&& request_info->http_headers[i].name[0] != '\0'); i++)
    lua_push_str_table_entry(L,
			     request_info->http_headers[i].name,
			     (char*)request_info->http_headers[i].value);  
  lua_setglobal(L, (char*)"_SERVER");

  /* Cookies */
  lua_newtable(L);
  if((_cookies = (char*)mg_get_header(conn, "Cookie")) != NULL) {
    char *cookies = strdup(_cookies);
    char *tok, *where;

    // ntop->getTrace()->traceEvent(TRACE_WARNING, "=> '%s'", cookies);
    tok = strtok_r(cookies, "=", &where);
    while(tok != NULL) {
      char *val;

      while(tok[0] == ' ') tok++;

      if((val = strtok_r(NULL, ";", &where)) != NULL) {
	lua_push_str_table_entry(L, tok, val);
	// ntop->getTrace()->traceEvent(TRACE_WARNING, "'%s'='%s'", tok, val);
      } else
	break;

      tok = strtok_r(NULL, "=", &where);
    }

    free(cookies);
  }
  lua_setglobal(L, "_COOKIE"); /* Like in php */

  /* Put the _SESSION params into the environment */
  lua_newtable(L);

  mg_get_cookie(conn, "user", user, sizeof(user));
  lua_push_str_table_entry(L, "user", user);
  mg_get_cookie(conn, "session", buf, sizeof(buf));
  lua_push_str_table_entry(L, "session", buf);

  // now it's time to set the interface.
  setInterface(user);

  lua_setglobal(L, "_SESSION"); /* Like in php */

  if(user[0] != '\0') {
    char val[255];

    lua_pushlightuserdata(L, user);
    lua_setglobal(L, "user");

    snprintf(key, sizeof(key), "ntopng.user.%s.allowed_nets", user);
    if((ntop->getRedis()->get(key, val, sizeof(val)) != -1)
       && (val[0] != '\0')) {
      ptree.addAddresses(val);
      lua_pushlightuserdata(L, &ptree);
      lua_setglobal(L, CONST_ALLOWED_NETS);
      // ntop->getTrace()->traceEvent(TRACE_WARNING, "SET %p", ptree);
    }

    snprintf(key, sizeof(key), CONST_STR_USER_ALLOWED_IFNAME, user);
    if(snprintf(key, sizeof(key), CONST_STR_USER_ALLOWED_IFNAME, user)
       && !ntop->getRedis()->get(key, ifname, sizeof(ifname))) {
      lua_pushlightuserdata(L, ifname);
      lua_setglobal(L, CONST_ALLOWED_IFNAME);
    }
  }

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
    return(send_error(conn, 500 /* Internal server error */,
		      "Internal server error", PAGE_ERROR, script_path, err));
  }

  return(CONST_LUA_OK);
}
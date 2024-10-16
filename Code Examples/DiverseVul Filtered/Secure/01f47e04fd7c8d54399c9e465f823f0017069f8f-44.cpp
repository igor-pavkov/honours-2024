static int ntop_zmq_receive(lua_State* vm) {
  NetworkInterface *ntop_interface = getCurrentInterface(vm);
  void *subscriber;
  int size;
  struct zmq_msg_hdr h;
  char *payload;
  int payload_len;
  zmq_pollitem_t item;
  int rc;

  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_getglobal(vm, "zmq_subscriber");
  if((subscriber = (void*)lua_touserdata(vm, lua_gettop(vm))) == NULL) {
    ntop->getTrace()->traceEvent(TRACE_ERROR, "INTERNAL ERROR: NULL subscriber");
    return(CONST_LUA_ERROR);
  }

  item.socket = subscriber;
  item.events = ZMQ_POLLIN;
  do {
    rc = zmq_poll(&item, 1, 1000);
    if(rc < 0 || !ntop_interface->isRunning()) /* CHECK */
      return(CONST_LUA_PARAM_ERROR);
  } while (rc == 0);

  size = zmq_recv(subscriber, &h, sizeof(h), 0);

  if(size != sizeof(h) || h.version != ZMQ_MSG_VERSION) {
    ntop->getTrace()->traceEvent(TRACE_WARNING, "Unsupported publisher version [%d]", h.version);
    return -1;
  }

  payload_len = h.size + 1;
  if((payload = (char*)malloc(payload_len)) != NULL) {
    size = zmq_recv(subscriber, payload, payload_len, 0);
    payload[h.size] = '\0';

    if(size > 0) {
      enum json_tokener_error jerr = json_tokener_success;
      json_object *o = json_tokener_parse_verbose(payload, &jerr);

      if(o != NULL) {
	struct json_object_iterator it = json_object_iter_begin(o);
	struct json_object_iterator itEnd = json_object_iter_end(o);

	while (!json_object_iter_equal(&it, &itEnd)) {
	  char *key   = (char*)json_object_iter_peek_name(&it);
	  const char *value = json_object_get_string(json_object_iter_peek_value(&it));

	  ntop->getTrace()->traceEvent(TRACE_NORMAL, "[%s]=[%s]", key, value);

	  json_object_iter_next(&it);
	}

	json_object_put(o);
      } else
	ntop->getTrace()->traceEvent(TRACE_WARNING, "JSON Parse error [%s]: %s",
				     json_tokener_error_desc(jerr),
				     payload);

      lua_pushfstring(vm, "%s", payload);
      ntop->getTrace()->traceEvent(TRACE_INFO, "[%u] %s", h.size, payload);
      free(payload);
      return(CONST_LUA_OK);
    } else {
      free(payload);
      return(CONST_LUA_PARAM_ERROR);
    }
  } else
    return(CONST_LUA_PARAM_ERROR);
}
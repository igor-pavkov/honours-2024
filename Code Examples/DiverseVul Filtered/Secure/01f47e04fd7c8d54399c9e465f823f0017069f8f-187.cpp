static int ntop_get_ndpi_categories(lua_State* vm) {
  ntop->getTrace()->traceEvent(TRACE_DEBUG, "%s() called", __FUNCTION__);

  lua_newtable(vm);

  for (int i=0; i < NDPI_PROTOCOL_NUM_CATEGORIES; i++) {
    char buf[8];

    snprintf(buf, sizeof(buf), "%d", i);
    lua_push_str_table_entry(vm, ndpi_category_str((ndpi_protocol_category_t)i), buf);
  }

  return(CONST_LUA_OK);
}
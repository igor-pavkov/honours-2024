static int lua_ap_scoreboard_worker(lua_State *L)
{
    int i, j;
    worker_score *ws_record = NULL;
    request_rec *r = NULL;

    luaL_checktype(L, 1, LUA_TUSERDATA);
    luaL_checktype(L, 2, LUA_TNUMBER);
    luaL_checktype(L, 3, LUA_TNUMBER);

    r = ap_lua_check_request_rec(L, 1);
    if (!r) return 0;

    i = lua_tointeger(L, 2);
    j = lua_tointeger(L, 3);
    ws_record = apr_palloc(r->pool, sizeof *ws_record);

    ap_copy_scoreboard_worker(ws_record, i, j);
    if (ws_record) {
        lua_newtable(L);

        lua_pushstring(L, "access_count");
        lua_pushnumber(L, ws_record->access_count);
        lua_settable(L, -3);

        lua_pushstring(L, "bytes_served");
        lua_pushnumber(L, (lua_Number) ws_record->bytes_served);
        lua_settable(L, -3);

        lua_pushstring(L, "client");
        lua_pushstring(L, ws_record->client);
        lua_settable(L, -3);

        lua_pushstring(L, "conn_bytes");
        lua_pushnumber(L, (lua_Number) ws_record->conn_bytes);
        lua_settable(L, -3);

        lua_pushstring(L, "conn_count");
        lua_pushnumber(L, ws_record->conn_count);
        lua_settable(L, -3);

        lua_pushstring(L, "generation");
        lua_pushnumber(L, ws_record->generation);
        lua_settable(L, -3);

        lua_pushstring(L, "last_used");
        lua_pushnumber(L, (lua_Number) ws_record->last_used);
        lua_settable(L, -3);

        lua_pushstring(L, "pid");
        lua_pushnumber(L, ws_record->pid);
        lua_settable(L, -3);

        lua_pushstring(L, "request");
        lua_pushstring(L, ws_record->request);
        lua_settable(L, -3);

        lua_pushstring(L, "start_time");
        lua_pushnumber(L, (lua_Number) ws_record->start_time);
        lua_settable(L, -3);

        lua_pushstring(L, "status");
        lua_pushnumber(L, ws_record->status);
        lua_settable(L, -3);

        lua_pushstring(L, "stop_time");
        lua_pushnumber(L, (lua_Number) ws_record->stop_time);
        lua_settable(L, -3);

        lua_pushstring(L, "tid");

        lua_pushinteger(L, (lua_Integer) ws_record->tid);
        lua_settable(L, -3);

        lua_pushstring(L, "vhost");
        lua_pushstring(L, ws_record->vhost);
        lua_settable(L, -3);
#ifdef HAVE_TIMES
        lua_pushstring(L, "stimes");
        lua_pushnumber(L, ws_record->times.tms_stime);
        lua_settable(L, -3);

        lua_pushstring(L, "utimes");
        lua_pushnumber(L, ws_record->times.tms_utime);
        lua_settable(L, -3);
#endif
        return 1;
    }
    return 0;
}
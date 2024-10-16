void ap_lua_rstack_dump(lua_State *L, request_rec *r, const char *msg)
{
    int i;
    int top = lua_gettop(L);
    ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, APLOGNO(01484) "Lua Stack Dump: [%s]", msg);
    for (i = 1; i <= top; i++) {
        int t = lua_type(L, i);
        switch (t) {
        case LUA_TSTRING:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  '%s'", i, lua_tostring(L, i));
                break;
            }
        case LUA_TUSERDATA:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "%d:  userdata",
                              i);
                break;
            }
        case LUA_TLIGHTUSERDATA:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  lightuserdata", i);
                break;
            }
        case LUA_TNIL:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "%d:  NIL", i);
                break;
            }
        case LUA_TNONE:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r, "%d:  None", i);
                break;
            }
        case LUA_TBOOLEAN:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  %s", i, lua_toboolean(L,
                                                          i) ? "true" :
                              "false");
                break;
            }
        case LUA_TNUMBER:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  %g", i, lua_tonumber(L, i));
                break;
            }
        case LUA_TTABLE:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  <table>", i);
                break;
            }
        case LUA_TFUNCTION:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  <function>", i);
                break;
            }
        default:{
                ap_log_rerror(APLOG_MARK, APLOG_INFO, 0, r,
                              "%d:  unknown: -[%s]-", i, lua_typename(L, i));
                break;
            }
        }
    }
}
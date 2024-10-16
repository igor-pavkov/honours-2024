static int req_dispatch(lua_State *L)
{
    apr_hash_t *dispatch;
    req_fun_t *rft;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    const char *name = luaL_checkstring(L, 2);
    lua_pop(L, 2);

    lua_getfield(L, LUA_REGISTRYINDEX, "Apache2.Request.dispatch");
    dispatch = lua_touserdata(L, 1);
    lua_pop(L, 1);

    rft = apr_hash_get(dispatch, name, APR_HASH_KEY_STRING);
    if (rft) {
        switch (rft->type) {
        case APL_REQ_FUNTYPE_TABLE:{
                req_table_t *rs;
                req_field_apr_table_f func = (req_field_apr_table_f)rft->fun;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01486)
                              "request_rec->dispatching %s -> apr table",
                              name);
                rs = (*func)(r);
                ap_lua_push_apr_table(L, rs);
                return 1;
            }

        case APL_REQ_FUNTYPE_LUACFUN:{
                lua_CFunction func = (lua_CFunction)rft->fun;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01487)
                              "request_rec->dispatching %s -> lua_CFunction",
                              name);
                lua_pushcfunction(L, func);
                return 1;
            }
        case APL_REQ_FUNTYPE_STRING:{
                req_field_string_f func = (req_field_string_f)rft->fun;
                char *rs;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01488)
                              "request_rec->dispatching %s -> string", name);
                rs = (*func) (r);
                lua_pushstring(L, rs);
                return 1;
            }
        case APL_REQ_FUNTYPE_INT:{
                req_field_int_f func = (req_field_int_f)rft->fun;
                int rs;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01489)
                              "request_rec->dispatching %s -> int", name);
                rs = (*func) (r);
                lua_pushinteger(L, rs);
                return 1;
            }
        case APL_REQ_FUNTYPE_BOOLEAN:{
                req_field_int_f func = (req_field_int_f)rft->fun;
                int rs;
                ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01490)
                              "request_rec->dispatching %s -> boolean", name);
                rs = (*func) (r);
                lua_pushboolean(L, rs);
                return 1;
            }
        }
    }

    ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, APLOGNO(01491) "nothing for %s", name);
    return 0;
}
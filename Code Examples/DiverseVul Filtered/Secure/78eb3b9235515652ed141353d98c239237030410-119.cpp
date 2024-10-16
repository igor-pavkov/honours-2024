void ap_lua_load_request_lmodule(lua_State *L, apr_pool_t *p)
{

    apr_hash_t *dispatch = apr_hash_make(p);

    apr_hash_set(dispatch, "puts", APR_HASH_KEY_STRING,
                 makefun(&req_puts, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "write", APR_HASH_KEY_STRING,
                 makefun(&req_write, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "document_root", APR_HASH_KEY_STRING,
                 makefun(&req_document_root, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "context_prefix", APR_HASH_KEY_STRING,
                 makefun(&req_context_prefix, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "context_document_root", APR_HASH_KEY_STRING,
                 makefun(&req_context_document_root, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "parseargs", APR_HASH_KEY_STRING,
                 makefun(&req_parseargs, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "parsebody", APR_HASH_KEY_STRING,
                 makefun(&req_parsebody, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "debug", APR_HASH_KEY_STRING,
                 makefun(&req_debug, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "info", APR_HASH_KEY_STRING,
                 makefun(&req_info, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "notice", APR_HASH_KEY_STRING,
                 makefun(&req_notice, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "warn", APR_HASH_KEY_STRING,
                 makefun(&req_warn, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "err", APR_HASH_KEY_STRING,
                 makefun(&req_err, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "crit", APR_HASH_KEY_STRING,
                 makefun(&req_crit, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "alert", APR_HASH_KEY_STRING,
                 makefun(&req_alert, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "emerg", APR_HASH_KEY_STRING,
                 makefun(&req_emerg, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace1", APR_HASH_KEY_STRING,
                 makefun(&req_trace1, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace2", APR_HASH_KEY_STRING,
                 makefun(&req_trace2, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace3", APR_HASH_KEY_STRING,
                 makefun(&req_trace3, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace4", APR_HASH_KEY_STRING,
                 makefun(&req_trace4, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace5", APR_HASH_KEY_STRING,
                 makefun(&req_trace5, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace6", APR_HASH_KEY_STRING,
                 makefun(&req_trace6, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace7", APR_HASH_KEY_STRING,
                 makefun(&req_trace7, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "trace8", APR_HASH_KEY_STRING,
                 makefun(&req_trace8, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "add_output_filter", APR_HASH_KEY_STRING,
                 makefun(&req_add_output_filter, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "construct_url", APR_HASH_KEY_STRING,
                 makefun(&req_construct_url, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "escape_html", APR_HASH_KEY_STRING,
                 makefun(&req_escape_html, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "ssl_var_lookup", APR_HASH_KEY_STRING,
                 makefun(&req_ssl_var_lookup, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "is_https", APR_HASH_KEY_STRING,
                 makefun(&req_ssl_is_https_field, APL_REQ_FUNTYPE_BOOLEAN, p));
    apr_hash_set(dispatch, "assbackwards", APR_HASH_KEY_STRING,
                 makefun(&req_assbackwards_field, APL_REQ_FUNTYPE_BOOLEAN, p));
    apr_hash_set(dispatch, "status", APR_HASH_KEY_STRING,
                 makefun(&req_status_field, APL_REQ_FUNTYPE_INT, p));
    apr_hash_set(dispatch, "protocol", APR_HASH_KEY_STRING,
                 makefun(&req_protocol_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "range", APR_HASH_KEY_STRING,
                 makefun(&req_range_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "content_type", APR_HASH_KEY_STRING,
                 makefun(&req_content_type_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "content_encoding", APR_HASH_KEY_STRING,
                 makefun(&req_content_encoding_field, APL_REQ_FUNTYPE_STRING,
                         p));
    apr_hash_set(dispatch, "ap_auth_type", APR_HASH_KEY_STRING,
                 makefun(&req_ap_auth_type_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "unparsed_uri", APR_HASH_KEY_STRING,
                 makefun(&req_unparsed_uri_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "user", APR_HASH_KEY_STRING,
                 makefun(&req_user_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "filename", APR_HASH_KEY_STRING,
                 makefun(&req_filename_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "canonical_filename", APR_HASH_KEY_STRING,
                 makefun(&req_canonical_filename_field,
                         APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "path_info", APR_HASH_KEY_STRING,
                 makefun(&req_path_info_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "args", APR_HASH_KEY_STRING,
                 makefun(&req_args_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "handler", APR_HASH_KEY_STRING,
                 makefun(&req_handler_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "hostname", APR_HASH_KEY_STRING,
                 makefun(&req_hostname_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "uri", APR_HASH_KEY_STRING,
                 makefun(&req_uri_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "the_request", APR_HASH_KEY_STRING,
                 makefun(&req_the_request_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "log_id", APR_HASH_KEY_STRING,
                 makefun(&req_log_id_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "useragent_ip", APR_HASH_KEY_STRING,
                 makefun(&req_useragent_ip_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "method", APR_HASH_KEY_STRING,
                 makefun(&req_method_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "proxyreq", APR_HASH_KEY_STRING,
                 makefun(&req_proxyreq_field, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "headers_in", APR_HASH_KEY_STRING,
                 makefun(&req_headers_in, APL_REQ_FUNTYPE_TABLE, p));
    apr_hash_set(dispatch, "headers_out", APR_HASH_KEY_STRING,
                 makefun(&req_headers_out, APL_REQ_FUNTYPE_TABLE, p));
    apr_hash_set(dispatch, "err_headers_out", APR_HASH_KEY_STRING,
                 makefun(&req_err_headers_out, APL_REQ_FUNTYPE_TABLE, p));
    apr_hash_set(dispatch, "notes", APR_HASH_KEY_STRING,
                 makefun(&req_notes, APL_REQ_FUNTYPE_TABLE, p));
    apr_hash_set(dispatch, "subprocess_env", APR_HASH_KEY_STRING,
                 makefun(&req_subprocess_env, APL_REQ_FUNTYPE_TABLE, p));
    apr_hash_set(dispatch, "flush", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_rflush, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "port", APR_HASH_KEY_STRING,
                 makefun(&req_ap_get_server_port, APL_REQ_FUNTYPE_INT, p));
    apr_hash_set(dispatch, "banner", APR_HASH_KEY_STRING,
                 makefun(&ap_get_server_banner, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "options", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_options, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "allowoverrides", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_allowoverrides, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "started", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_started, APL_REQ_FUNTYPE_INT, p));
    apr_hash_set(dispatch, "basic_auth_pw", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_basic_auth_pw, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "limit_req_body", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_limit_req_body, APL_REQ_FUNTYPE_INT, p));
    apr_hash_set(dispatch, "server_built", APR_HASH_KEY_STRING,
                 makefun(&ap_get_server_built, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "is_initial_req", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_is_initial_req, APL_REQ_FUNTYPE_BOOLEAN, p));
    apr_hash_set(dispatch, "remaining", APR_HASH_KEY_STRING,
                 makefun(&req_remaining_field, APL_REQ_FUNTYPE_INT, p));
    apr_hash_set(dispatch, "some_auth_required", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_some_auth_required, APL_REQ_FUNTYPE_BOOLEAN, p));
    apr_hash_set(dispatch, "server_name", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_get_server_name, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "auth_name", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_auth_name, APL_REQ_FUNTYPE_STRING, p));
    apr_hash_set(dispatch, "sendfile", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_sendfile, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "dbacquire", APR_HASH_KEY_STRING,
                 makefun(&lua_db_acquire, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "stat", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_stat, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "get_direntries", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_getdir, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "regex", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_regex, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "usleep", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_usleep, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "base64_encode", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_b64encode, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "base64_decode", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_b64decode, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "md5", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_md5, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "sha1", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_sha1, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "htpassword", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_htpassword, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "touch", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_touch, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "mkdir", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_mkdir, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "mkrdir", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_mkrdir, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "rmdir", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_rmdir, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "date_parse_rfc", APR_HASH_KEY_STRING,
                 makefun(&lua_apr_date_parse_rfc, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "escape", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_escape, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "unescape", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_unescape, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "mpm_query", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_mpm_query, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "expr", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_expr, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "scoreboard_process", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_scoreboard_process, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "scoreboard_worker", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_scoreboard_worker, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "clock", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_clock, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "requestbody", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_requestbody, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "add_input_filter", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_add_input_filter, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "module_info", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_module_info, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "loaded_modules", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_loaded_modules, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "runtime_dir_relative", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_runtime_dir_relative, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "server_info", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_server_info, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "set_document_root", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_set_document_root, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "set_context_info", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_set_context_info, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "os_escape_path", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_os_escape_path, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "escape_logitem", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_escape_logitem, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "strcmp_match", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_strcmp_match, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "set_keepalive", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_set_keepalive, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "make_etag", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_make_etag, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "send_interim_response", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_send_interim_response, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "custom_response", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_custom_response, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "exists_config_define", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_exists_config_define, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "state_query", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_state_query, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "get_server_name_for_url", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_get_server_name_for_url, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "ivm_get", APR_HASH_KEY_STRING,
                 makefun(&lua_ivm_get, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "ivm_set", APR_HASH_KEY_STRING,
                 makefun(&lua_ivm_set, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "getcookie", APR_HASH_KEY_STRING,
                 makefun(&lua_get_cookie, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "setcookie", APR_HASH_KEY_STRING,
                 makefun(&lua_set_cookie, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "wsupgrade", APR_HASH_KEY_STRING,
                 makefun(&lua_websocket_greet, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "wsread", APR_HASH_KEY_STRING,
                 makefun(&lua_websocket_read, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "wspeek", APR_HASH_KEY_STRING,
                 makefun(&lua_websocket_peek, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "wswrite", APR_HASH_KEY_STRING,
                 makefun(&lua_websocket_write, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "wsclose", APR_HASH_KEY_STRING,
                 makefun(&lua_websocket_close, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "wsping", APR_HASH_KEY_STRING,
                 makefun(&lua_websocket_ping, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "config", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_get_config, APL_REQ_FUNTYPE_LUACFUN, p));
    apr_hash_set(dispatch, "activeconfig", APR_HASH_KEY_STRING,
                 makefun(&lua_ap_get_active_config, APL_REQ_FUNTYPE_LUACFUN, p));
    lua_pushlightuserdata(L, dispatch);
    lua_setfield(L, LUA_REGISTRYINDEX, "Apache2.Request.dispatch");

    luaL_newmetatable(L, "Apache2.Request");    /* [metatable] */
    lua_pushvalue(L, -1);

    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, request_methods);    /* [metatable] */

    lua_pop(L, 2);

    luaL_newmetatable(L, "Apache2.Connection"); /* [metatable] */
    lua_pushvalue(L, -1);

    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, connection_methods); /* [metatable] */

    lua_pop(L, 2);

    luaL_newmetatable(L, "Apache2.Server");     /* [metatable] */
    lua_pushvalue(L, -1);

    lua_setfield(L, -2, "__index");
    luaL_register(L, NULL, server_methods);     /* [metatable] */

    lua_pop(L, 2);

}
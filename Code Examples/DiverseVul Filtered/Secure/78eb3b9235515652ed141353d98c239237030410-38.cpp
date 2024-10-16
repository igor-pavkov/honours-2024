static int lua_websocket_write(lua_State *L) 
{
    const char *string;
    apr_status_t rv;
    size_t len;
    int raw = 0;
    char prelude;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    
    if (lua_isboolean(L, 3)) {
        raw = lua_toboolean(L, 3);
    }
    string = lua_tolstring(L, 2, &len);
    
    if (raw != 1) {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, 
                        "Websocket: Writing framed message to client");
        
        prelude = 0x81; /* text frame, FIN */
        ap_rputc(prelude, r);
        if (len < 126) {
            ap_rputc(len, r);
        } 
        else if (len < 65535) {
            apr_uint16_t slen = len;
            ap_rputc(126, r); 
            slen = htons(slen);
            ap_rwrite((char*) &slen, 2, r);
        }
        else {
            apr_uint64_t llen = len;
            ap_rputc(127, r);
            llen = ap_ntoh64(&llen); /* ntoh doubles as hton */
            ap_rwrite((char*) &llen, 8, r);
        }
    }
    else {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, 
                        "Websocket: Writing raw message to client");
    }
    ap_rwrite(string, len, r);
    rv = ap_rflush(r);
    if (rv == APR_SUCCESS) {
        lua_pushboolean(L, 1);
    }
    else {
        lua_pushboolean(L, 0);
    }
    return 1;
}
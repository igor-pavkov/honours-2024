static int lua_websocket_ping(lua_State *L) 
{
    apr_socket_t *sock;
    apr_size_t plen;
    char prelude[2];
    apr_status_t rv;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    sock = ap_get_conn_socket(r->connection);
    
    /* Send a header that says: PING. */
    prelude[0] = 0x89; /* ping  opcode */
    prelude[1] = 0;
    plen = 2;
    apr_socket_send(sock, prelude, &plen);
    
    
    /* Get opcode and FIN bit from pong */
    plen = 2;
    rv = apr_socket_recv(sock, prelude, &plen);
    if (rv == APR_SUCCESS) {
        unsigned char opcode = prelude[0];
        unsigned char len = prelude[1];
        unsigned char mask = len >> 7;
        if (mask) len -= 128;
        plen = len;
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, 
                        "Websocket: Got PONG opcode: %x", opcode);
        if (opcode == 0x8A) {
            lua_pushboolean(L, 1);
        }
        else {
            lua_pushboolean(L, 0);
        }
        if (plen > 0) {
            ap_log_rerror(APLOG_MARK, APLOG_TRACE1, 0, r, 
                        "Websocket: Reading %" APR_SIZE_T_FMT " bytes of PONG", plen);
            return 1;
        }
        if (mask) {
            plen = 2;
            apr_socket_recv(sock, prelude, &plen);
            plen = 2;
            apr_socket_recv(sock, prelude, &plen);
        }
    }
    else {
        lua_pushboolean(L, 0);
    }
    return 1;
}
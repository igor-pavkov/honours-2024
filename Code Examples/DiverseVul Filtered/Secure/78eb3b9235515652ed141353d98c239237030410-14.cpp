static int lua_websocket_close(lua_State *L) 
{
    apr_socket_t *sock;
    char prelude[2];
    request_rec *r = ap_lua_check_request_rec(L, 1);
    
    sock = ap_get_conn_socket(r->connection);
    
    /* Send a header that says: socket is closing. */
    prelude[0] = 0x88; /* closing socket opcode */
    prelude[1] = 0; /* zero length frame */
    ap_rwrite(prelude, 2, r);
    
    /* Close up tell the MPM and filters to back off */
    apr_socket_close(sock);
    r->output_filters = NULL;
    r->connection->keepalive = AP_CONN_CLOSE;
    return 0;
}
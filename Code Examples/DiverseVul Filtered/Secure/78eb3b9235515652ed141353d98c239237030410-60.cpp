static int lua_websocket_greet(lua_State *L)
{
    const char *key = NULL;
    unsigned char digest[APR_SHA1_DIGESTSIZE];
    apr_sha1_ctx_t sha1;
    char           *encoded;
    int encoded_len;
    request_rec *r = ap_lua_check_request_rec(L, 1);
    key = apr_table_get(r->headers_in, "Sec-WebSocket-Key");
    if (key != NULL) {
        ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, 
                    "Websocket: Got websocket key: %s", key);
        key = apr_pstrcat(r->pool, key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11", 
                NULL);
        apr_sha1_init(&sha1);
        apr_sha1_update(&sha1, key, strlen(key));
        apr_sha1_final(digest, &sha1);
        encoded_len = apr_base64_encode_len(APR_SHA1_DIGESTSIZE);
        if (encoded_len) {
            encoded = apr_palloc(r->pool, encoded_len);
            encoded_len = apr_base64_encode(encoded, (char*) digest, APR_SHA1_DIGESTSIZE);
            r->status = 101;
            apr_table_set(r->headers_out, "Upgrade", "websocket");
            apr_table_set(r->headers_out, "Connection", "Upgrade");
            apr_table_set(r->headers_out, "Sec-WebSocket-Accept", encoded);
            
            /* Trick httpd into NOT using the chunked filter, IMPORTANT!!!111*/
            apr_table_set(r->headers_out, "Transfer-Encoding", "chunked");
            
            r->clength = 0;
            r->bytes_sent = 0;
            r->read_chunked = 0;
            ap_rflush(r);
            ap_log_rerror(APLOG_MARK, APLOG_DEBUG, 0, r, 
                          "Websocket: Upgraded from HTTP to Websocket");
            lua_pushboolean(L, 1);
            return 1;
        }
    }
    ap_log_rerror(APLOG_MARK, APLOG_NOTICE, 0, r, APLOGNO(02666)
                  "Websocket: Upgrade from HTTP to Websocket failed");
    return 0;
}
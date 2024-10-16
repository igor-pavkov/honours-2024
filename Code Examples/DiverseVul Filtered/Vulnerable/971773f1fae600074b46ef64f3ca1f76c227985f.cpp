static handler_t wstunnel_handler_setup (request_st * const r, plugin_data * const p) {
    handler_ctx *hctx = r->plugin_ctx[p->id];
    int hybivers;
    hctx->errh = r->conf.errh;/*(for mod_wstunnel-specific DEBUG_* macros)*/
    hctx->conf = p->conf; /*(copies struct)*/
    hybivers = wstunnel_check_request(r, hctx);
    if (hybivers < 0) return HANDLER_FINISHED;
    hctx->hybivers = hybivers;
    if (0 == hybivers) {
        DEBUG_LOG_INFO("WebSocket Version = %s", "hybi-00");
    }
    else {
        DEBUG_LOG_INFO("WebSocket Version = %d", hybivers);
    }

    hctx->gw.opts.backend     = BACKEND_PROXY; /*(act proxy-like)*/
    hctx->gw.opts.pdata       = hctx;
    hctx->gw.opts.parse       = wstunnel_recv_parse;
    hctx->gw.stdin_append     = wstunnel_stdin_append;
    hctx->gw.create_env       = wstunnel_create_env;
    hctx->gw.handler_ctx_free = wstunnel_handler_ctx_free;
    hctx->gw.backend_error    = wstunnel_backend_error;
    hctx->gw.response         = chunk_buffer_acquire();

    hctx->frame.state         = MOD_WEBSOCKET_FRAME_STATE_INIT;
    hctx->frame.ctl.siz       = 0;
    hctx->frame.payload       = chunk_buffer_acquire();

    unsigned int binary = hctx->conf.frame_type; /*(0 = "text"; 1 = "binary")*/
    if (!binary) {
        const buffer *vb =
          http_header_request_get(r, HTTP_HEADER_OTHER, CONST_STR_LEN("Sec-WebSocket-Protocol"));
        if (NULL != vb) {
            for (const char *s = vb->ptr; *s; ++s) {
                while (*s==' '||*s=='\t'||*s=='\r'||*s=='\n') ++s;
                if (buffer_eq_icase_ssn(s, CONST_STR_LEN("binary"))) {
                    s += sizeof("binary")-1;
                    while (*s==' '||*s=='\t'||*s=='\r'||*s=='\n') ++s;
                    if (*s==','||*s=='\0') {
                        hctx->subproto = 1;
                        binary = 1;
                        break;
                    }
                }
                else if (buffer_eq_icase_ssn(s, CONST_STR_LEN("base64"))) {
                    s += sizeof("base64")-1;
                    while (*s==' '||*s=='\t'||*s=='\r'||*s=='\n') ++s;
                    if (*s==','||*s=='\0') {
                        hctx->subproto = -1;
                        break;
                    }
                }
                s = strchr(s, ',');
                if (NULL == s) break;
            }
        }
    }

    if (binary) {
        DEBUG_LOG_INFO("%s", "will recv binary data from backend");
        hctx->frame.type         = MOD_WEBSOCKET_FRAME_TYPE_BIN;
        hctx->frame.type_before  = MOD_WEBSOCKET_FRAME_TYPE_BIN;
        hctx->frame.type_backend = MOD_WEBSOCKET_FRAME_TYPE_BIN;
    }
    else {
        DEBUG_LOG_INFO("%s", "will recv text data from backend");
        hctx->frame.type         = MOD_WEBSOCKET_FRAME_TYPE_TEXT;
        hctx->frame.type_before  = MOD_WEBSOCKET_FRAME_TYPE_TEXT;
        hctx->frame.type_backend = MOD_WEBSOCKET_FRAME_TYPE_TEXT;
    }

    return HANDLER_GO_ON;
}
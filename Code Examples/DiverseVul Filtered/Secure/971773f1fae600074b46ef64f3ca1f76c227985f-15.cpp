static int send_rfc_6455(handler_ctx *hctx, mod_wstunnel_frame_type_t type, const char *payload, size_t siz) {
    char mem[10];
    size_t len;

    /* allowed null payload for ping, pong, close frame */
    if (payload == NULL && (   type == MOD_WEBSOCKET_FRAME_TYPE_TEXT
                            || type == MOD_WEBSOCKET_FRAME_TYPE_BIN   )) {
        return -1;
    }

    switch (type) {
    case MOD_WEBSOCKET_FRAME_TYPE_TEXT:
        mem[0] = (char)(0x80 | MOD_WEBSOCKET_OPCODE_TEXT);
        DEBUG_LOG_DEBUG("%s", "type = text");
        break;
    case MOD_WEBSOCKET_FRAME_TYPE_BIN:
        mem[0] = (char)(0x80 | MOD_WEBSOCKET_OPCODE_BIN);
        DEBUG_LOG_DEBUG("%s", "type = binary");
        break;
    case MOD_WEBSOCKET_FRAME_TYPE_PING:
        mem[0] = (char) (0x80 | MOD_WEBSOCKET_OPCODE_PING);
        DEBUG_LOG_DEBUG("%s", "type = ping");
        break;
    case MOD_WEBSOCKET_FRAME_TYPE_PONG:
        mem[0] = (char)(0x80 | MOD_WEBSOCKET_OPCODE_PONG);
        DEBUG_LOG_DEBUG("%s", "type = pong");
        break;
    case MOD_WEBSOCKET_FRAME_TYPE_CLOSE:
    default:
        mem[0] = (char)(0x80 | MOD_WEBSOCKET_OPCODE_CLOSE);
        DEBUG_LOG_DEBUG("%s", "type = close");
        break;
    }

    DEBUG_LOG_DEBUG("payload size=%zx", siz);
    if (siz < MOD_WEBSOCKET_FRAME_LEN16) {
        mem[1] = siz;
        len = 2;
    }
    else if (siz <= UINT16_MAX) {
        mem[1] = MOD_WEBSOCKET_FRAME_LEN16;
        mem[2] = (siz >> 8) & 0xff;
        mem[3] = siz & 0xff;
        len = 1+MOD_WEBSOCKET_FRAME_LEN16_CNT+1;
    }
    else {
        mem[1] = MOD_WEBSOCKET_FRAME_LEN63;
        mem[2] = 0;
        mem[3] = 0;
        mem[4] = 0;
        mem[5] = 0;
        mem[6] = (siz >> 24) & 0xff;
        mem[7] = (siz >> 16) & 0xff;
        mem[8] = (siz >> 8) & 0xff;
        mem[9] = siz & 0xff;
        len = 1+MOD_WEBSOCKET_FRAME_LEN63_CNT+1;
    }
    request_st * const r = hctx->gw.r;
    http_chunk_append_mem(r, mem, len);
    if (siz) http_chunk_append_mem(r, payload, siz);
    DEBUG_LOG_DEBUG("send data to client (fd=%d), frame size=%zx",
                    r->con->fd, len+siz);
    return 0;
}
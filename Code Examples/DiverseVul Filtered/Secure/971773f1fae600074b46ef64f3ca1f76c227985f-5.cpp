static int send_ietf_00(handler_ctx *hctx, mod_wstunnel_frame_type_t type, const char *payload, size_t siz) {
    static const char head =  0; /* 0x00 */
    static const char tail = ~0; /* 0xff */
    request_st * const r = hctx->gw.r;
    char *mem;
    size_t len;

    switch (type) {
    case MOD_WEBSOCKET_FRAME_TYPE_TEXT:
        if (0 == siz) return 0;
        http_chunk_append_mem(r, &head, 1);
        http_chunk_append_mem(r, payload, siz);
        http_chunk_append_mem(r, &tail, 1);
        len = siz+2;
        break;
    case MOD_WEBSOCKET_FRAME_TYPE_BIN:
        if (0 == siz) return 0;
        http_chunk_append_mem(r, &head, 1);
        len = 4*(siz/3)+4+1;
        /* avoid accumulating too much data in memory; send to tmpfile */
        mem = malloc(len);
        force_assert(mem);
        len=li_to_base64(mem,len,(unsigned char *)payload,siz,BASE64_STANDARD);
        http_chunk_append_mem(r, mem, len);
        free(mem);
        http_chunk_append_mem(r, &tail, 1);
        len += 2;
        break;
    case MOD_WEBSOCKET_FRAME_TYPE_CLOSE:
        http_chunk_append_mem(r, &tail, 1);
        http_chunk_append_mem(r, &head, 1);
        len = 2;
        break;
    default:
        DEBUG_LOG_ERR("%s", "invalid frame type");
        return -1;
    }
    DEBUG_LOG_DEBUG("send data to client (fd=%d), frame size=%zx",
                    r->con->fd, len);
    return 0;
}
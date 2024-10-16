static int recv_ietf_00(handler_ctx *hctx) {
    request_st * const r = hctx->gw.r;
    chunkqueue *cq = &r->reqbody_queue;
    buffer *payload = hctx->frame.payload;
    char *mem;
    DEBUG_LOG_DEBUG("recv data from client (fd=%d), size=%llx",
                    r->con->fd, (long long)chunkqueue_length(cq));
    for (chunk *c = cq->first; c; c = c->next) {
        char *frame = c->mem->ptr+c->offset;
        /*(chunk_remaining_length() on MEM_CHUNK)*/
        size_t flen = (size_t)(buffer_clen(c->mem) - c->offset);
        /*(FILE_CHUNK not handled, but might need to add support)*/
        force_assert(c->type == MEM_CHUNK);
        for (size_t i = 0; i < flen; ) {
            switch (hctx->frame.state) {
            case MOD_WEBSOCKET_FRAME_STATE_INIT:
                hctx->frame.ctl.siz = 0;
                if (frame[i] == 0x00) {
                    hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_READ_PAYLOAD;
                    i++;
                }
                else if (((unsigned char *)frame)[i] == 0xff) {
                    DEBUG_LOG_DEBUG("%s", "recv close frame");
                    return -1;
                }
                else {
                    DEBUG_LOG_DEBUG("%s", "recv invalid frame");
                    return -1;
                }
                break;
            case MOD_WEBSOCKET_FRAME_STATE_READ_PAYLOAD:
                mem = (char *)memchr(frame+i, 0xff, flen - i);
                if (mem == NULL) {
                    DEBUG_LOG_DEBUG("got continuous payload, size=%zx", flen-i);
                    hctx->frame.ctl.siz += flen - i;
                    if (hctx->frame.ctl.siz > MOD_WEBSOCKET_BUFMAX) {
                        DEBUG_LOG_WARN("frame size has been exceeded: %x",
                                       MOD_WEBSOCKET_BUFMAX);
                        return -1;
                    }
                    buffer_append_string_len(payload, frame+i, flen - i);
                    i += flen - i;
                }
                else {
                    DEBUG_LOG_DEBUG("got final payload, size=%zx",
                                    mem - (frame+i));
                    hctx->frame.ctl.siz += (mem - (frame+i));
                    if (hctx->frame.ctl.siz > MOD_WEBSOCKET_BUFMAX) {
                        DEBUG_LOG_WARN("frame size has been exceeded: %x",
                                       MOD_WEBSOCKET_BUFMAX);
                        return -1;
                    }
                    buffer_append_string_len(payload, frame+i, mem - (frame+i));
                    i += (mem - (frame+i));
                    hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_INIT;
                }
                i++;
                if (hctx->frame.type == MOD_WEBSOCKET_FRAME_TYPE_TEXT
                    && !buffer_is_unset(payload)) { /*XXX: buffer_is_blank?*/
                    hctx->frame.ctl.siz = 0;
                    chunkqueue_append_buffer(&hctx->gw.wb, payload);
                    /*buffer_clear(payload);*//*chunkqueue_append_buffer clear*/
                }
                else {
                    if (hctx->frame.state == MOD_WEBSOCKET_FRAME_STATE_INIT
                        && !buffer_is_unset(payload)) {/*XXX: buffer_is_blank?*/
                        buffer *b;
                        size_t len = buffer_clen(payload);
                        len = (len+3)/4*3+1;
                        chunkqueue_get_memory(&hctx->gw.wb, &len);
                        b = hctx->gw.wb.last->mem;
                        len = buffer_clen(b);
                        DEBUG_LOG_DEBUG("try to base64 decode: %s",
                                        payload->ptr);
                        if (NULL ==
                            buffer_append_base64_decode(b, BUF_PTR_LEN(payload),
                                                        BASE64_STANDARD)) {
                            DEBUG_LOG_ERR("%s", "fail to base64-decode");
                            return -1;
                        }
                        buffer_clear(payload);
                        /*chunkqueue_use_memory()*/
                        hctx->gw.wb.bytes_in += buffer_clen(b)-len;
                    }
                }
                break;
            default: /* never reach */
                DEBUG_LOG_ERR("%s", "BUG: unknown state");
                return -1;
            }
        }
    }
    /* XXX: should add ability to handle and preserve partial frames above */
    /*(not chunkqueue_reset(); do not reset cq->bytes_in, cq->bytes_out)*/
    chunkqueue_mark_written(cq, chunkqueue_length(cq));
    return 0;
}
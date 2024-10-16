static int recv_rfc_6455(handler_ctx *hctx) {
    request_st * const r = hctx->gw.r;
    chunkqueue *cq = &r->reqbody_queue;
    buffer *payload = hctx->frame.payload;
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
                switch (frame[i] & 0x0f) {
                case MOD_WEBSOCKET_OPCODE_CONT:
                    DEBUG_LOG_DEBUG("%s", "type = continue");
                    hctx->frame.type = hctx->frame.type_before;
                    break;
                case MOD_WEBSOCKET_OPCODE_TEXT:
                    DEBUG_LOG_DEBUG("%s", "type = text");
                    hctx->frame.type = MOD_WEBSOCKET_FRAME_TYPE_TEXT;
                    hctx->frame.type_before = hctx->frame.type;
                    break;
                case MOD_WEBSOCKET_OPCODE_BIN:
                    DEBUG_LOG_DEBUG("%s", "type = binary");
                    hctx->frame.type = MOD_WEBSOCKET_FRAME_TYPE_BIN;
                    hctx->frame.type_before = hctx->frame.type;
                    break;
                case MOD_WEBSOCKET_OPCODE_PING:
                    DEBUG_LOG_DEBUG("%s", "type = ping");
                    hctx->frame.type = MOD_WEBSOCKET_FRAME_TYPE_PING;
                    break;
                case MOD_WEBSOCKET_OPCODE_PONG:
                    DEBUG_LOG_DEBUG("%s", "type = pong");
                    hctx->frame.type = MOD_WEBSOCKET_FRAME_TYPE_PONG;
                    break;
                case MOD_WEBSOCKET_OPCODE_CLOSE:
                    DEBUG_LOG_DEBUG("%s", "type = close");
                    hctx->frame.type = MOD_WEBSOCKET_FRAME_TYPE_CLOSE;
                    return -1;
                    break;
                default:
                    DEBUG_LOG_ERR("%s", "type is invalid");
                    return -1;
                    break;
                }
                i++;
                hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_READ_LENGTH;
                break;
            case MOD_WEBSOCKET_FRAME_STATE_READ_LENGTH:
                if ((frame[i] & 0x80) != 0x80) {
                    DEBUG_LOG_ERR("%s", "payload was not masked");
                    return -1;
                }
                hctx->frame.ctl.mask_cnt = 0;
                hctx->frame.ctl.siz = (uint64_t)(frame[i] & 0x7f);
                if (hctx->frame.ctl.siz == 0) {
                    DEBUG_LOG_DEBUG("specified payload size=%llx",
                                    (unsigned long long)hctx->frame.ctl.siz);
                    hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_READ_MASK;
                }
                else if (hctx->frame.ctl.siz == MOD_WEBSOCKET_FRAME_LEN16) {
                    hctx->frame.ctl.siz = 0;
                    hctx->frame.ctl.siz_cnt = MOD_WEBSOCKET_FRAME_LEN16_CNT;
                    hctx->frame.state =
                        MOD_WEBSOCKET_FRAME_STATE_READ_EX_LENGTH;
                }
                else if (hctx->frame.ctl.siz == MOD_WEBSOCKET_FRAME_LEN63) {
                    hctx->frame.ctl.siz = 0;
                    hctx->frame.ctl.siz_cnt = MOD_WEBSOCKET_FRAME_LEN63_CNT;
                    hctx->frame.state =
                        MOD_WEBSOCKET_FRAME_STATE_READ_EX_LENGTH;
                }
                else {
                    DEBUG_LOG_DEBUG("specified payload size=%llx",
                                    (unsigned long long)hctx->frame.ctl.siz);
                    hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_READ_MASK;
                }
                i++;
                break;
            case MOD_WEBSOCKET_FRAME_STATE_READ_EX_LENGTH:
                hctx->frame.ctl.siz =
                    (hctx->frame.ctl.siz << 8) + (frame[i] & 0xff);
                hctx->frame.ctl.siz_cnt--;
                if (hctx->frame.ctl.siz_cnt <= 0) {
                    if (hctx->frame.type == MOD_WEBSOCKET_FRAME_TYPE_PING &&
                        hctx->frame.ctl.siz > MOD_WEBSOCKET_BUFMAX) {
                        DEBUG_LOG_WARN("frame size has been exceeded: %x",
                                       MOD_WEBSOCKET_BUFMAX);
                        return -1;
                    }
                    DEBUG_LOG_DEBUG("specified payload size=%llx",
                                    (unsigned long long)hctx->frame.ctl.siz);
                    hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_READ_MASK;
                }
                i++;
                break;
            case MOD_WEBSOCKET_FRAME_STATE_READ_MASK:
                hctx->frame.ctl.mask[hctx->frame.ctl.mask_cnt] = frame[i];
                hctx->frame.ctl.mask_cnt++;
                if (hctx->frame.ctl.mask_cnt >= MOD_WEBSOCKET_MASK_CNT) {
                    hctx->frame.ctl.mask_cnt = 0;
                    if (hctx->frame.type == MOD_WEBSOCKET_FRAME_TYPE_PING &&
                        hctx->frame.ctl.siz == 0) {
                        mod_wstunnel_frame_send(hctx,
                                                MOD_WEBSOCKET_FRAME_TYPE_PONG,
                                                NULL, 0);
                    }
                    if (hctx->frame.ctl.siz == 0) {
                        hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_INIT;
                    }
                    else {
                        hctx->frame.state =
                            MOD_WEBSOCKET_FRAME_STATE_READ_PAYLOAD;
                    }
                }
                i++;
                break;
            case MOD_WEBSOCKET_FRAME_STATE_READ_PAYLOAD:
                /* hctx->frame.ctl.siz <= SIZE_MAX */
                if (hctx->frame.ctl.siz <= flen - i) {
                    DEBUG_LOG_DEBUG("read payload, size=%llx",
                                    (unsigned long long)hctx->frame.ctl.siz);
                    buffer_append_string_len(payload, frame+i, (size_t)
                                             (hctx->frame.ctl.siz & SIZE_MAX));
                    i += (size_t)(hctx->frame.ctl.siz & SIZE_MAX);
                    hctx->frame.ctl.siz = 0;
                    hctx->frame.state = MOD_WEBSOCKET_FRAME_STATE_INIT;
                    DEBUG_LOG_DEBUG("rest of frame size=%zx", flen - i);
                /* SIZE_MAX < hctx->frame.ctl.siz */
                }
                else {
                    DEBUG_LOG_DEBUG("read payload, size=%zx", flen - i);
                    buffer_append_string_len(payload, frame+i, flen - i);
                    hctx->frame.ctl.siz -= flen - i;
                    i += flen - i;
                    DEBUG_LOG_DEBUG("rest of payload size=%llx",
                                    (unsigned long long)hctx->frame.ctl.siz);
                }
                switch (hctx->frame.type) {
                case MOD_WEBSOCKET_FRAME_TYPE_TEXT:
                case MOD_WEBSOCKET_FRAME_TYPE_BIN:
                  {
                    unmask_payload(hctx);
                    chunkqueue_append_buffer(&hctx->gw.wb, payload);
                    /*buffer_clear(payload);*//*chunkqueue_append_buffer clear*/
                    break;
                  }
                case MOD_WEBSOCKET_FRAME_TYPE_PING:
                    if (hctx->frame.ctl.siz == 0) {
                        unmask_payload(hctx);
                        mod_wstunnel_frame_send(hctx,
                          MOD_WEBSOCKET_FRAME_TYPE_PONG,
                          payload->ptr, buffer_clen(payload));
                        buffer_clear(payload);
                    }
                    break;
                case MOD_WEBSOCKET_FRAME_TYPE_PONG:
                    buffer_clear(payload);
                    break;
                case MOD_WEBSOCKET_FRAME_TYPE_CLOSE:
                default:
                    DEBUG_LOG_ERR("%s", "BUG: invalid frame type");
                    return -1;
                }
                break;
            default:
                DEBUG_LOG_ERR("%s", "BUG: invalid state");
                return -1;
            }
        }
    }
    /* XXX: should add ability to handle and preserve partial frames above */
    /*(not chunkqueue_reset(); do not reset cq->bytes_in, cq->bytes_out)*/
    chunkqueue_mark_written(cq, chunkqueue_length(cq));
    return 0;
}
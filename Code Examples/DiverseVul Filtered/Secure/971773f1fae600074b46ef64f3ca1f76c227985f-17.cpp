static int get_key3(request_st * const r, char *buf, uint32_t bytes) {
    /* 8 bytes should have been sent with request
     * for draft-ietf-hybi-thewebsocketprotocol-00 */
    chunkqueue *cq = &r->reqbody_queue;
    /*(caller should ensure bytes available prior to calling this routine)*/
    /*assert(chunkqueue_length(cq) >= 8);*/
    /*assert(8 == bytes);*/
    return chunkqueue_read_data(cq, buf, bytes, r->conf.errh);
}
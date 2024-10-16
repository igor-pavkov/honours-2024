void _addReplyStringToList(client *c, const char *s, size_t len) {
    robj *tail;

    if (c->flags & CLIENT_CLOSE_AFTER_REPLY) return;

    if (listLength(c->reply) == 0) {
        robj *o = createStringObject(s,len);

        listAddNodeTail(c->reply,o);
        c->reply_bytes += getStringObjectSdsUsedMemory(o);
    } else {
        tail = listNodeValue(listLast(c->reply));

        /* Append to this object when possible. */
        if (tail->ptr != NULL && tail->encoding == OBJ_ENCODING_RAW &&
            sdslen(tail->ptr)+len <= PROTO_REPLY_CHUNK_BYTES)
        {
            c->reply_bytes -= sdsZmallocSize(tail->ptr);
            tail = dupLastObjectIfNeeded(c->reply);
            tail->ptr = sdscatlen(tail->ptr,s,len);
            c->reply_bytes += sdsZmallocSize(tail->ptr);
        } else {
            robj *o = createStringObject(s,len);

            listAddNodeTail(c->reply,o);
            c->reply_bytes += getStringObjectSdsUsedMemory(o);
        }
    }
    asyncCloseClientOnOutputBufferLimitReached(c);
}
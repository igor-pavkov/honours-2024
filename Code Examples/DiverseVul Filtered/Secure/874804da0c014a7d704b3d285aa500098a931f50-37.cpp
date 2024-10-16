void _addReplyObjectToList(client *c, robj *o) {
    robj *tail;

    if (c->flags & CLIENT_CLOSE_AFTER_REPLY) return;

    if (listLength(c->reply) == 0) {
        incrRefCount(o);
        listAddNodeTail(c->reply,o);
        c->reply_bytes += getStringObjectSdsUsedMemory(o);
    } else {
        tail = listNodeValue(listLast(c->reply));

        /* Append to this object when possible. */
        if (tail->ptr != NULL &&
            tail->encoding == OBJ_ENCODING_RAW &&
            sdslen(tail->ptr)+sdslen(o->ptr) <= PROTO_REPLY_CHUNK_BYTES)
        {
            c->reply_bytes -= sdsZmallocSize(tail->ptr);
            tail = dupLastObjectIfNeeded(c->reply);
            tail->ptr = sdscatlen(tail->ptr,o->ptr,sdslen(o->ptr));
            c->reply_bytes += sdsZmallocSize(tail->ptr);
        } else {
            incrRefCount(o);
            listAddNodeTail(c->reply,o);
            c->reply_bytes += getStringObjectSdsUsedMemory(o);
        }
    }
    asyncCloseClientOnOutputBufferLimitReached(c);
}
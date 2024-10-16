void _addReplySdsToList(client *c, sds s) {
    robj *tail;

    if (c->flags & CLIENT_CLOSE_AFTER_REPLY) {
        sdsfree(s);
        return;
    }

    if (listLength(c->reply) == 0) {
        listAddNodeTail(c->reply,createObject(OBJ_STRING,s));
        c->reply_bytes += sdsZmallocSize(s);
    } else {
        tail = listNodeValue(listLast(c->reply));

        /* Append to this object when possible. */
        if (tail->ptr != NULL && tail->encoding == OBJ_ENCODING_RAW &&
            sdslen(tail->ptr)+sdslen(s) <= PROTO_REPLY_CHUNK_BYTES)
        {
            c->reply_bytes -= sdsZmallocSize(tail->ptr);
            tail = dupLastObjectIfNeeded(c->reply);
            tail->ptr = sdscatlen(tail->ptr,s,sdslen(s));
            c->reply_bytes += sdsZmallocSize(tail->ptr);
            sdsfree(s);
        } else {
            listAddNodeTail(c->reply,createObject(OBJ_STRING,s));
            c->reply_bytes += sdsZmallocSize(s);
        }
    }
    asyncCloseClientOnOutputBufferLimitReached(c);
}
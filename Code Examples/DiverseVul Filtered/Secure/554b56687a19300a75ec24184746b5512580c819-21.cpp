static int add_chunked_item_iovs(conn *c, item *it, int len) {
    assert(it->it_flags & ITEM_CHUNKED);
    item_chunk *ch = (item_chunk *) ITEM_schunk(it);
    while (ch) {
        int todo = (len > ch->used) ? ch->used : len;
        if (add_iov(c, ch->data, todo) != 0) {
            return -1;
        }
        ch = ch->next;
        len -= todo;
    }
    return 0;
}
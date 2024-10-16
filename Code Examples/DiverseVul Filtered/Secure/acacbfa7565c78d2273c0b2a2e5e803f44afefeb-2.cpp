static int grow_init_buf(SSL *s, size_t size) {

    size_t msg_offset = (char *)s->init_msg - s->init_buf->data;

    if (!BUF_MEM_grow_clean(s->init_buf, (int)size))
        return 0;

    if (size < msg_offset)
        return 0;

    s->init_msg = s->init_buf->data + msg_offset;

    return 1;
}
int ssl3_send_next_proto(SSL *s)
{
    unsigned int len, padding_len;
    unsigned char *d;

    if (s->state == SSL3_ST_CW_NEXT_PROTO_A) {
        len = s->next_proto_negotiated_len;
        padding_len = 32 - ((len + 2) % 32);
        d = (unsigned char *)s->init_buf->data;
        d[4] = len;
        memcpy(d + 5, s->next_proto_negotiated, len);
        d[5 + len] = padding_len;
        memset(d + 6 + len, 0, padding_len);
        *(d++) = SSL3_MT_NEXT_PROTO;
        l2n3(2 + len + padding_len, d);
        s->state = SSL3_ST_CW_NEXT_PROTO_B;
        s->init_num = 4 + 2 + len + padding_len;
        s->init_off = 0;
    }

    return ssl3_do_write(s, SSL3_RT_HANDSHAKE);
}
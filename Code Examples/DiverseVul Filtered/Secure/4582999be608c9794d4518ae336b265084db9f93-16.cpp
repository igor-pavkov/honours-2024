void ssl_set_bio( ssl_context *ssl,
            int (*f_recv)(void *, unsigned char *, size_t), void *p_recv,
            int (*f_send)(void *, const unsigned char *, size_t), void *p_send )
{
    ssl->f_recv     = f_recv;
    ssl->f_send     = f_send;
    ssl->p_recv     = p_recv;
    ssl->p_send     = p_send;
}
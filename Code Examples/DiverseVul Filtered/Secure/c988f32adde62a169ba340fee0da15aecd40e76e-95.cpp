void ssl_set_psk_cb( ssl_context *ssl,
                     int (*f_psk)(void *, ssl_context *, const unsigned char *,
                     size_t),
                     void *p_psk )
{
    ssl->f_psk = f_psk;
    ssl->p_psk = p_psk;
}
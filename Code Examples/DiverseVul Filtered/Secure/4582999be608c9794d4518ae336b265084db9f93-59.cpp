void ssl_set_sni( ssl_context *ssl,
                  int (*f_sni)(void *, ssl_context *,
                                const unsigned char *, size_t),
                  void *p_sni )
{
    ssl->f_sni = f_sni;
    ssl->p_sni = p_sni;
}
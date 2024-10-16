void ssl_set_dbg( ssl_context *ssl,
                  void (*f_dbg)(void *, int, const char *),
                  void  *p_dbg )
{
    ssl->f_dbg      = f_dbg;
    ssl->p_dbg      = p_dbg;
}
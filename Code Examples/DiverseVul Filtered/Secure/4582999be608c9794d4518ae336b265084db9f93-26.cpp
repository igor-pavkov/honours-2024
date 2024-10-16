void ssl_set_session_cache( ssl_context *ssl,
        int (*f_get_cache)(void *, ssl_session *), void *p_get_cache,
        int (*f_set_cache)(void *, const ssl_session *), void *p_set_cache )
{
    ssl->f_get_cache = f_get_cache;
    ssl->p_get_cache = p_get_cache;
    ssl->f_set_cache = f_set_cache;
    ssl->p_set_cache = p_set_cache;
}
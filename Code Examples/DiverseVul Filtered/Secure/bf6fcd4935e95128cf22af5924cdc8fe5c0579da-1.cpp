void pure_memzero(void * const pnt, const size_t len)
{
# ifdef HAVE_EXPLICIT_BZERO
    explicit_bzero(pnt, len);
# else
    volatile unsigned char *pnt_ = (volatile unsigned char *) pnt;
    size_t                     i = (size_t) 0U;

    while (i < len) {
        pnt_[i++] = 0U;
    }
# endif
}
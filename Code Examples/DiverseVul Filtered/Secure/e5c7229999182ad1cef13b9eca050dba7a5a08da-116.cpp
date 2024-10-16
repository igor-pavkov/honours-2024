FF_SYMVER(void*, av_fast_realloc, (void *ptr, unsigned int *size, size_t min_size), LIBNAME)
{
    return av_fast_realloc(ptr, size, min_size);
}
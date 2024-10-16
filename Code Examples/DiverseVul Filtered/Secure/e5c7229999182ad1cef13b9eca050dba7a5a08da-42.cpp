FF_SYMVER(void, av_fast_malloc, (void *ptr, unsigned int *size, size_t min_size), LIBNAME)
{
    av_fast_malloc(ptr, size, min_size);
}
void av_fast_padded_malloc(void *ptr, unsigned int *size, size_t min_size)
{
    uint8_t **p = ptr;
    if (min_size > SIZE_MAX - FF_INPUT_BUFFER_PADDING_SIZE) {
        av_freep(p);
        *size = 0;
        return;
    }
    if (!ff_fast_malloc(p, size, min_size + FF_INPUT_BUFFER_PADDING_SIZE, 1))
        memset(*p + min_size, 0, FF_INPUT_BUFFER_PADDING_SIZE);
}
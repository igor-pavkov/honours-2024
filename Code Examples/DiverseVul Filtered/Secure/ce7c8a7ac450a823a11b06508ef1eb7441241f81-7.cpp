char *_q_strcpy(char *dst, size_t size, const char *src)
{
    if (dst == NULL || size == 0 || src == NULL) return dst;

    size_t copylen = strlen(src);
    if (copylen >= size) copylen = size - 1;
    memmove((void *)dst, (void *)src, copylen);
    dst[copylen] = '\0';

    return dst;
}
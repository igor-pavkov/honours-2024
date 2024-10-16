bool pb_decode_fixed32(pb_istream_t *stream, void *dest)
{
    union {
        uint32_t fixed32;
        pb_byte_t bytes[4];
    } u;

    if (!pb_read(stream, u.bytes, 4))
        return false;

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN && CHAR_BIT == 8
    /* fast path - if we know that we're on little endian, assign directly */
    *(uint32_t*)dest = u.fixed32;
#else
    *(uint32_t*)dest = ((uint32_t)u.bytes[0] << 0) |
                       ((uint32_t)u.bytes[1] << 8) |
                       ((uint32_t)u.bytes[2] << 16) |
                       ((uint32_t)u.bytes[3] << 24);
#endif
    return true;
}
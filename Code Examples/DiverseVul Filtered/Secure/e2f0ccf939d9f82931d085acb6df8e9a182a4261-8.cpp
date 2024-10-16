bool pb_decode_fixed64(pb_istream_t *stream, void *dest)
{
    union {
        uint64_t fixed64;
        pb_byte_t bytes[8];
    } u;

    if (!pb_read(stream, u.bytes, 8))
        return false;

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN && CHAR_BIT == 8
    /* fast path - if we know that we're on little endian, assign directly */
    *(uint64_t*)dest = u.fixed64;
#else
    *(uint64_t*)dest = ((uint64_t)u.bytes[0] << 0) |
                       ((uint64_t)u.bytes[1] << 8) |
                       ((uint64_t)u.bytes[2] << 16) |
                       ((uint64_t)u.bytes[3] << 24) |
                       ((uint64_t)u.bytes[4] << 32) |
                       ((uint64_t)u.bytes[5] << 40) |
                       ((uint64_t)u.bytes[6] << 48) |
                       ((uint64_t)u.bytes[7] << 56);
#endif
    return true;
}
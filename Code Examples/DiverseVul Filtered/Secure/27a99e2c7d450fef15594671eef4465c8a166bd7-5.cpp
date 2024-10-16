static uint8_t *read_vblock(AVIOContext *src, uint32_t *size,
                            uint32_t key, uint32_t *k2, int align)
{
    uint8_t tmp[4];
    uint8_t *buf;
    unsigned n;

    if (avio_read(src, tmp, 4) != 4)
        return NULL;

    decode_block(tmp, tmp, 4, key, k2, align);

    n = get_v(tmp, 4);
    if (n < 4)
        return NULL;

    buf = av_malloc(n);
    if (!buf)
        return NULL;

    *size = n;
    n -= 4;

    memcpy(buf, tmp, 4);

    if (avio_read(src, buf + 4, n) == n) {
        decode_block(buf + 4, buf + 4, n, key, k2, align);
    } else {
        av_free(buf);
        buf = NULL;
    }

    return buf;
}
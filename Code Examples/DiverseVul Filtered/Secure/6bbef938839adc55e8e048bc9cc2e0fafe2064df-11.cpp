static inline void mpeg4_encode_dc(PutBitContext *s, int level, int n)
{
#if 1
    /* DC will overflow if level is outside the [-255,255] range. */
    level += 256;
    if (n < 4) {
        /* luminance */
        put_bits(s, uni_DCtab_lum_len[level], uni_DCtab_lum_bits[level]);
    } else {
        /* chrominance */
        put_bits(s, uni_DCtab_chrom_len[level], uni_DCtab_chrom_bits[level]);
    }
#else
    int size, v;
    /* find number of bits */
    size = 0;
    v    = abs(level);
    while (v) {
        v >>= 1;
        size++;
    }

    if (n < 4) {
        /* luminance */
        put_bits(s, ff_mpeg4_DCtab_lum[size][1], ff_mpeg4_DCtab_lum[size][0]);
    } else {
        /* chrominance */
        put_bits(s, ff_mpeg4_DCtab_chrom[size][1], ff_mpeg4_DCtab_chrom[size][0]);
    }

    /* encode remaining bits */
    if (size > 0) {
        if (level < 0)
            level = (-level) ^ ((1 << size) - 1);
        put_bits(s, size, level);
        if (size > 8)
            put_bits(s, 1, 1);
    }
#endif
}
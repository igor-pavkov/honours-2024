static av_cold void init_uni_dc_tab(void)
{
    int level, uni_code, uni_len;

    for (level = -256; level < 256; level++) {
        int size, v, l;
        /* find number of bits */
        size = 0;
        v    = abs(level);
        while (v) {
            v >>= 1;
            size++;
        }

        if (level < 0)
            l = (-level) ^ ((1 << size) - 1);
        else
            l = level;

        /* luminance */
        uni_code = ff_mpeg4_DCtab_lum[size][0];
        uni_len  = ff_mpeg4_DCtab_lum[size][1];

        if (size > 0) {
            uni_code <<= size;
            uni_code  |= l;
            uni_len   += size;
            if (size > 8) {
                uni_code <<= 1;
                uni_code  |= 1;
                uni_len++;
            }
        }
        uni_DCtab_lum_bits[level + 256] = uni_code;
        uni_DCtab_lum_len[level + 256]  = uni_len;

        /* chrominance */
        uni_code = ff_mpeg4_DCtab_chrom[size][0];
        uni_len  = ff_mpeg4_DCtab_chrom[size][1];

        if (size > 0) {
            uni_code <<= size;
            uni_code  |= l;
            uni_len   += size;
            if (size > 8) {
                uni_code <<= 1;
                uni_code  |= 1;
                uni_len++;
            }
        }
        uni_DCtab_chrom_bits[level + 256] = uni_code;
        uni_DCtab_chrom_len[level + 256]  = uni_len;
    }
}
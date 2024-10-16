static inline int mpeg4_get_dc_length(int level, int n)
{
    if (n < 4)
        return uni_DCtab_lum_len[level + 256];
    else
        return uni_DCtab_chrom_len[level + 256];
}
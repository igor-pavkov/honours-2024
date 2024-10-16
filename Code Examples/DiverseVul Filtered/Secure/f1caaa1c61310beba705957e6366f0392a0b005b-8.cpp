static av_always_inline int dnxhd_calc_ac_bits(DNXHDEncContext *ctx, int16_t *block, int last_index)
{
    int last_non_zero = 0;
    int bits = 0;
    int i, j, level;
    for (i = 1; i <= last_index; i++) {
        j = ctx->m.intra_scantable.permutated[i];
        level = block[j];
        if (level) {
            int run_level = i - last_non_zero - 1;
            bits += ctx->vlc_bits[(level<<1)|!!run_level]+ctx->run_bits[run_level];
            last_non_zero = i;
        }
    }
    return bits;
}
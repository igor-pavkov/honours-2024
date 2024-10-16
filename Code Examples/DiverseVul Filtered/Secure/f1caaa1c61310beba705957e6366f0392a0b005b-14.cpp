static av_always_inline void dnxhd_encode_block(DNXHDEncContext *ctx, int16_t *block, int last_index, int n)
{
    int last_non_zero = 0;
    int slevel, i, j;

    dnxhd_encode_dc(ctx, block[0] - ctx->m.last_dc[n]);
    ctx->m.last_dc[n] = block[0];

    for (i = 1; i <= last_index; i++) {
        j = ctx->m.intra_scantable.permutated[i];
        slevel = block[j];
        if (slevel) {
            int run_level = i - last_non_zero - 1;
            int rlevel = (slevel<<1)|!!run_level;
            put_bits(&ctx->m.pb, ctx->vlc_bits[rlevel], ctx->vlc_codes[rlevel]);
            if (run_level)
                put_bits(&ctx->m.pb, ctx->run_bits[run_level], ctx->run_codes[run_level]);
            last_non_zero = i;
        }
    }
    put_bits(&ctx->m.pb, ctx->vlc_bits[0], ctx->vlc_codes[0]); // EOB
}
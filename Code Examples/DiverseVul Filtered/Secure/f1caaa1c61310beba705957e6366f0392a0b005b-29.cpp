static av_always_inline void dnxhd_encode_dc(DNXHDEncContext *ctx, int diff)
{
    int nbits;
    if (diff < 0) {
        nbits = av_log2_16bit(-2*diff);
        diff--;
    } else {
        nbits = av_log2_16bit(2*diff);
    }
    put_bits(&ctx->m.pb, ctx->cid_table->dc_bits[nbits] + nbits,
             (ctx->cid_table->dc_codes[nbits]<<nbits) + (diff & ((1 << nbits) - 1)));
}
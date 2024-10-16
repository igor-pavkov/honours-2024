static int dnxhd_calc_bits_thread(AVCodecContext *avctx, void *arg, int jobnr, int threadnr)
{
    DNXHDEncContext *ctx = avctx->priv_data;
    int mb_y = jobnr, mb_x;
    int qscale = ctx->qscale;
    LOCAL_ALIGNED_16(int16_t, block, [64]);
    ctx = ctx->thread[threadnr];

    ctx->m.last_dc[0] =
    ctx->m.last_dc[1] =
    ctx->m.last_dc[2] = 1 << (ctx->cid_table->bit_depth + 2);

    for (mb_x = 0; mb_x < ctx->m.mb_width; mb_x++) {
        unsigned mb = mb_y * ctx->m.mb_width + mb_x;
        int ssd     = 0;
        int ac_bits = 0;
        int dc_bits = 0;
        int i;

        dnxhd_get_blocks(ctx, mb_x, mb_y);

        for (i = 0; i < 8; i++) {
            int16_t *src_block = ctx->blocks[i];
            int overflow, nbits, diff, last_index;
            int n = dnxhd_switch_matrix(ctx, i);

            memcpy(block, src_block, 64*sizeof(*block));
            last_index = ctx->m.dct_quantize(&ctx->m, block, 4&(2*i), qscale, &overflow);
            ac_bits += dnxhd_calc_ac_bits(ctx, block, last_index);

            diff = block[0] - ctx->m.last_dc[n];
            if (diff < 0) nbits = av_log2_16bit(-2*diff);
            else          nbits = av_log2_16bit( 2*diff);

            av_assert1(nbits < ctx->cid_table->bit_depth + 4);
            dc_bits += ctx->cid_table->dc_bits[nbits] + nbits;

            ctx->m.last_dc[n] = block[0];

            if (avctx->mb_decision == FF_MB_DECISION_RD || !RC_VARIANCE) {
                dnxhd_unquantize_c(ctx, block, i, qscale, last_index);
                ctx->m.dsp.idct(block);
                ssd += dnxhd_ssd_block(block, src_block);
            }
        }
        ctx->mb_rc[qscale][mb].ssd = ssd;
        ctx->mb_rc[qscale][mb].bits = ac_bits+dc_bits+12+8*ctx->vlc_bits[0];
    }
    return 0;
}
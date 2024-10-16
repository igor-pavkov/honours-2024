static av_always_inline void dnxhd_get_blocks(DNXHDEncContext *ctx, int mb_x, int mb_y)
{
    const int bs = ctx->block_width_l2;
    const int bw = 1 << bs;
    const uint8_t *ptr_y = ctx->thread[0]->src[0] + ((mb_y << 4) * ctx->m.linesize)   + (mb_x << bs+1);
    const uint8_t *ptr_u = ctx->thread[0]->src[1] + ((mb_y << 4) * ctx->m.uvlinesize) + (mb_x << bs);
    const uint8_t *ptr_v = ctx->thread[0]->src[2] + ((mb_y << 4) * ctx->m.uvlinesize) + (mb_x << bs);
    DSPContext *dsp = &ctx->m.dsp;

    dsp->get_pixels(ctx->blocks[0], ptr_y,      ctx->m.linesize);
    dsp->get_pixels(ctx->blocks[1], ptr_y + bw, ctx->m.linesize);
    dsp->get_pixels(ctx->blocks[2], ptr_u,      ctx->m.uvlinesize);
    dsp->get_pixels(ctx->blocks[3], ptr_v,      ctx->m.uvlinesize);

    if (mb_y+1 == ctx->m.mb_height && ctx->m.avctx->height == 1080) {
        if (ctx->interlaced) {
            ctx->get_pixels_8x4_sym(ctx->blocks[4], ptr_y + ctx->dct_y_offset,      ctx->m.linesize);
            ctx->get_pixels_8x4_sym(ctx->blocks[5], ptr_y + ctx->dct_y_offset + bw, ctx->m.linesize);
            ctx->get_pixels_8x4_sym(ctx->blocks[6], ptr_u + ctx->dct_uv_offset,     ctx->m.uvlinesize);
            ctx->get_pixels_8x4_sym(ctx->blocks[7], ptr_v + ctx->dct_uv_offset,     ctx->m.uvlinesize);
        } else {
            dsp->clear_block(ctx->blocks[4]);
            dsp->clear_block(ctx->blocks[5]);
            dsp->clear_block(ctx->blocks[6]);
            dsp->clear_block(ctx->blocks[7]);
        }
    } else {
        dsp->get_pixels(ctx->blocks[4], ptr_y + ctx->dct_y_offset,      ctx->m.linesize);
        dsp->get_pixels(ctx->blocks[5], ptr_y + ctx->dct_y_offset + bw, ctx->m.linesize);
        dsp->get_pixels(ctx->blocks[6], ptr_u + ctx->dct_uv_offset,     ctx->m.uvlinesize);
        dsp->get_pixels(ctx->blocks[7], ptr_v + ctx->dct_uv_offset,     ctx->m.uvlinesize);
    }
}
static void dnxhd_load_picture(DNXHDEncContext *ctx, const AVFrame *frame)
{
    int i;

    for (i = 0; i < ctx->m.avctx->thread_count; i++) {
        ctx->thread[i]->m.linesize    = frame->linesize[0] << ctx->interlaced;
        ctx->thread[i]->m.uvlinesize  = frame->linesize[1] << ctx->interlaced;
        ctx->thread[i]->dct_y_offset  = ctx->m.linesize  *8;
        ctx->thread[i]->dct_uv_offset = ctx->m.uvlinesize*8;
    }

    ctx->m.avctx->coded_frame->interlaced_frame = frame->interlaced_frame;
    ctx->cur_field = frame->interlaced_frame && !frame->top_field_first;
}
static int dnxhd_encode_picture(AVCodecContext *avctx, AVPacket *pkt,
                                const AVFrame *frame, int *got_packet)
{
    DNXHDEncContext *ctx = avctx->priv_data;
    int first_field = 1;
    int offset, i, ret;
    uint8_t *buf;

    if ((ret = ff_alloc_packet2(avctx, pkt, ctx->cid_table->frame_size)) < 0)
        return ret;
    buf = pkt->data;

    dnxhd_load_picture(ctx, frame);

 encode_coding_unit:
    for (i = 0; i < 3; i++) {
        ctx->src[i] = frame->data[i];
        if (ctx->interlaced && ctx->cur_field)
            ctx->src[i] += frame->linesize[i];
    }

    dnxhd_write_header(avctx, buf);

    if (avctx->mb_decision == FF_MB_DECISION_RD)
        ret = dnxhd_encode_rdo(avctx, ctx);
    else
        ret = dnxhd_encode_fast(avctx, ctx);
    if (ret < 0) {
        av_log(avctx, AV_LOG_ERROR,
               "picture could not fit ratecontrol constraints, increase qmax\n");
        return -1;
    }

    dnxhd_setup_threads_slices(ctx);

    offset = 0;
    for (i = 0; i < ctx->m.mb_height; i++) {
        AV_WB32(ctx->msip + i * 4, offset);
        offset += ctx->slice_size[i];
        av_assert1(!(ctx->slice_size[i] & 3));
    }

    avctx->execute2(avctx, dnxhd_encode_thread, buf, NULL, ctx->m.mb_height);

    av_assert1(640 + offset + 4 <= ctx->cid_table->coding_unit_size);
    memset(buf + 640 + offset, 0, ctx->cid_table->coding_unit_size - 4 - offset - 640);

    AV_WB32(buf + ctx->cid_table->coding_unit_size - 4, 0x600DC0DE); // EOF

    if (ctx->interlaced && first_field) {
        first_field     = 0;
        ctx->cur_field ^= 1;
        buf      += ctx->cid_table->coding_unit_size;
        goto encode_coding_unit;
    }

    avctx->coded_frame->quality = ctx->qscale * FF_QP2LAMBDA;

    pkt->flags |= AV_PKT_FLAG_KEY;
    *got_packet = 1;
    return 0;
}
static int video_get_buffer(AVCodecContext *s, AVFrame *pic)
{
    FramePool *pool = s->internal->pool;
    const AVPixFmtDescriptor *desc = av_pix_fmt_desc_get(pic->format);
    int pixel_size = desc->comp[0].step_minus1 + 1;
    int h_chroma_shift, v_chroma_shift;
    int i;

    if (pic->data[0] != NULL) {
        av_log(s, AV_LOG_ERROR, "pic->data[0]!=NULL in avcodec_default_get_buffer\n");
        return -1;
    }

    memset(pic->data, 0, sizeof(pic->data));
    pic->extended_data = pic->data;

    av_pix_fmt_get_chroma_sub_sample(s->pix_fmt, &h_chroma_shift, &v_chroma_shift);

    for (i = 0; i < 4 && pool->pools[i]; i++) {
        const int h_shift = i == 0 ? 0 : h_chroma_shift;
        const int v_shift = i == 0 ? 0 : v_chroma_shift;
        int is_planar = pool->pools[2] || (i==0 && s->pix_fmt == AV_PIX_FMT_GRAY8);

        pic->linesize[i] = pool->linesize[i];

        pic->buf[i] = av_buffer_pool_get(pool->pools[i]);
        if (!pic->buf[i])
            goto fail;

        // no edge if EDGE EMU or not planar YUV
        if ((s->flags & CODEC_FLAG_EMU_EDGE) || !is_planar)
            pic->data[i] = pic->buf[i]->data;
        else {
            pic->data[i] = pic->buf[i]->data +
                FFALIGN((pic->linesize[i] * EDGE_WIDTH >> v_shift) +
                        (pixel_size * EDGE_WIDTH >> h_shift), pool->stride_align[i]);
        }
    }
    for (; i < AV_NUM_DATA_POINTERS; i++) {
        pic->data[i] = NULL;
        pic->linesize[i] = 0;
    }
    if (pic->data[1] && !pic->data[2])
        avpriv_set_systematic_pal2((uint32_t *)pic->data[1], s->pix_fmt);

    if (s->debug & FF_DEBUG_BUFFERS)
        av_log(s, AV_LOG_DEBUG, "default_get_buffer called on pic %p\n", pic);

    return 0;
fail:
    av_frame_unref(pic);
    return AVERROR(ENOMEM);
}
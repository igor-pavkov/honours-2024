static int h264_frame_start(H264Context *h)
{
    Picture *pic;
    int i, ret;
    const int pixel_shift = h->pixel_shift;
    int c[4] = {
        1<<(h->sps.bit_depth_luma-1),
        1<<(h->sps.bit_depth_chroma-1),
        1<<(h->sps.bit_depth_chroma-1),
        -1
    };

    if (!ff_thread_can_start_frame(h->avctx)) {
        av_log(h->avctx, AV_LOG_ERROR, "Attempt to start a frame outside SETUP state\n");
        return -1;
    }

    release_unused_pictures(h, 1);
    h->cur_pic_ptr = NULL;

    i = find_unused_picture(h);
    if (i < 0) {
        av_log(h->avctx, AV_LOG_ERROR, "no frame buffer available\n");
        return i;
    }
    pic = &h->DPB[i];

    pic->reference              = h->droppable ? 0 : h->picture_structure;
    pic->f.coded_picture_number = h->coded_picture_number++;
    pic->field_picture          = h->picture_structure != PICT_FRAME;

    /*
     * Zero key_frame here; IDR markings per slice in frame or fields are ORed
     * in later.
     * See decode_nal_units().
     */
    pic->f.key_frame = 0;
    pic->mmco_reset  = 0;
    pic->recovered   = 0;

    if ((ret = alloc_picture(h, pic)) < 0)
        return ret;
    if(!h->frame_recovered && !h->avctx->hwaccel &&
       !(h->avctx->codec->capabilities & CODEC_CAP_HWACCEL_VDPAU))
        avpriv_color_frame(&pic->f, c);

    h->cur_pic_ptr = pic;
    unref_picture(h, &h->cur_pic);
    if (CONFIG_ERROR_RESILIENCE) {
        h->er.cur_pic = NULL;
    }

    if ((ret = ref_picture(h, &h->cur_pic, h->cur_pic_ptr)) < 0)
        return ret;

    if (CONFIG_ERROR_RESILIENCE) {
        ff_er_frame_start(&h->er);
        h->er.last_pic =
        h->er.next_pic = NULL;
    }

    assert(h->linesize && h->uvlinesize);

    for (i = 0; i < 16; i++) {
        h->block_offset[i]           = (4 * ((scan8[i] - scan8[0]) & 7) << pixel_shift) + 4 * h->linesize * ((scan8[i] - scan8[0]) >> 3);
        h->block_offset[48 + i]      = (4 * ((scan8[i] - scan8[0]) & 7) << pixel_shift) + 8 * h->linesize * ((scan8[i] - scan8[0]) >> 3);
    }
    for (i = 0; i < 16; i++) {
        h->block_offset[16 + i]      =
        h->block_offset[32 + i]      = (4 * ((scan8[i] - scan8[0]) & 7) << pixel_shift) + 4 * h->uvlinesize * ((scan8[i] - scan8[0]) >> 3);
        h->block_offset[48 + 16 + i] =
        h->block_offset[48 + 32 + i] = (4 * ((scan8[i] - scan8[0]) & 7) << pixel_shift) + 8 * h->uvlinesize * ((scan8[i] - scan8[0]) >> 3);
    }

    // s->decode = (h->flags & CODEC_FLAG_PSNR) || !s->encoding ||
    //             h->cur_pic.reference /* || h->contains_intra */ || 1;

    /* We mark the current picture as non-reference after allocating it, so
     * that if we break out due to an error it can be released automatically
     * in the next ff_MPV_frame_start().
     */
    h->cur_pic_ptr->reference = 0;

    h->cur_pic_ptr->field_poc[0] = h->cur_pic_ptr->field_poc[1] = INT_MAX;

    h->next_output_pic = NULL;

    assert(h->cur_pic_ptr->long_ref == 0);

    return 0;
}
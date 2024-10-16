static int decode_slice_header(FFV1Context *f, FFV1Context *fs)
{
    RangeCoder *c = &fs->c;
    uint8_t state[CONTEXT_SIZE];
    unsigned ps, i, context_count;
    memset(state, 128, sizeof(state));

    av_assert0(f->version > 2);

    fs->slice_x      =  get_symbol(c, state, 0)      * f->width ;
    fs->slice_y      =  get_symbol(c, state, 0)      * f->height;
    fs->slice_width  = (get_symbol(c, state, 0) + 1) * f->width  + fs->slice_x;
    fs->slice_height = (get_symbol(c, state, 0) + 1) * f->height + fs->slice_y;

    fs->slice_x /= f->num_h_slices;
    fs->slice_y /= f->num_v_slices;
    fs->slice_width  = fs->slice_width /f->num_h_slices - fs->slice_x;
    fs->slice_height = fs->slice_height/f->num_v_slices - fs->slice_y;
    if ((unsigned)fs->slice_width > f->width || (unsigned)fs->slice_height > f->height)
        return -1;
    if (    (unsigned)fs->slice_x + (uint64_t)fs->slice_width  > f->width
         || (unsigned)fs->slice_y + (uint64_t)fs->slice_height > f->height)
        return -1;

    for (i = 0; i < f->plane_count; i++) {
        PlaneContext * const p = &fs->plane[i];
        int idx = get_symbol(c, state, 0);
        if (idx >= (unsigned)f->quant_table_count) {
            av_log(f->avctx, AV_LOG_ERROR, "quant_table_index out of range\n");
            return -1;
        }
        p->quant_table_index = idx;
        memcpy(p->quant_table, f->quant_tables[idx], sizeof(p->quant_table));
        context_count = f->context_count[idx];

        if (p->context_count < context_count) {
            av_freep(&p->state);
            av_freep(&p->vlc_state);
        }
        p->context_count = context_count;
    }

    ps = get_symbol(c, state, 0);
    if (ps == 1) {
        f->cur->interlaced_frame = 1;
        f->cur->top_field_first  = 1;
    } else if (ps == 2) {
        f->cur->interlaced_frame = 1;
        f->cur->top_field_first  = 0;
    } else if (ps == 3) {
        f->cur->interlaced_frame = 0;
    }
    f->cur->sample_aspect_ratio.num = get_symbol(c, state, 0);
    f->cur->sample_aspect_ratio.den = get_symbol(c, state, 0);

    if (av_image_check_sar(f->width, f->height,
                           f->cur->sample_aspect_ratio) < 0) {
        av_log(f->avctx, AV_LOG_WARNING, "ignoring invalid SAR: %u/%u\n",
               f->cur->sample_aspect_ratio.num,
               f->cur->sample_aspect_ratio.den);
        f->cur->sample_aspect_ratio = (AVRational){ 0, 1 };
    }

    if (fs->version > 3) {
        fs->slice_reset_contexts = get_rac(c, state);
        fs->slice_coding_mode = get_symbol(c, state, 0);
        if (fs->slice_coding_mode != 1) {
            fs->slice_rct_by_coef = get_symbol(c, state, 0);
            fs->slice_rct_ry_coef = get_symbol(c, state, 0);
            if ((uint64_t)fs->slice_rct_by_coef + (uint64_t)fs->slice_rct_ry_coef > 4) {
                av_log(f->avctx, AV_LOG_ERROR, "slice_rct_y_coef out of range\n");
                return AVERROR_INVALIDDATA;
            }
        }
    }

    return 0;
}
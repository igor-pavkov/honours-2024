static void h264_er_decode_mb(void *opaque, int ref, int mv_dir, int mv_type,
                              int (*mv)[2][4][2],
                              int mb_x, int mb_y, int mb_intra, int mb_skipped)
{
    H264Context *h = opaque;

    h->mb_x  = mb_x;
    h->mb_y  = mb_y;
    h->mb_xy = mb_x + mb_y * h->mb_stride;
    memset(h->non_zero_count_cache, 0, sizeof(h->non_zero_count_cache));
    av_assert1(ref >= 0);
    /* FIXME: It is possible albeit uncommon that slice references
     * differ between slices. We take the easy approach and ignore
     * it for now. If this turns out to have any relevance in
     * practice then correct remapping should be added. */
    if (ref >= h->ref_count[0])
        ref = 0;
    if (!h->ref_list[0][ref].f.data[0]) {
        av_log(h->avctx, AV_LOG_DEBUG, "Reference not available for error concealing\n");
        ref = 0;
    }
    if ((h->ref_list[0][ref].reference&3) != 3) {
        av_log(h->avctx, AV_LOG_DEBUG, "Reference invalid\n");
        return;
    }
    fill_rectangle(&h->cur_pic.ref_index[0][4 * h->mb_xy],
                   2, 2, 2, ref, 1);
    fill_rectangle(&h->ref_cache[0][scan8[0]], 4, 4, 8, ref, 1);
    fill_rectangle(h->mv_cache[0][scan8[0]], 4, 4, 8,
                   pack16to32((*mv)[0][0][0], (*mv)[0][0][1]), 4);
    h->mb_mbaff =
    h->mb_field_decoding_flag = 0;
    ff_h264_hl_decode_mb(h);
}
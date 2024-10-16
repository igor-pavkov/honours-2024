static av_always_inline void mc_part_weighted(H264Context *h, int n, int square,
                                              int height, int delta,
                                              uint8_t *dest_y, uint8_t *dest_cb,
                                              uint8_t *dest_cr,
                                              int x_offset, int y_offset,
                                              qpel_mc_func *qpix_put,
                                              h264_chroma_mc_func chroma_put,
                                              h264_weight_func luma_weight_op,
                                              h264_weight_func chroma_weight_op,
                                              h264_biweight_func luma_weight_avg,
                                              h264_biweight_func chroma_weight_avg,
                                              int list0, int list1,
                                              int pixel_shift, int chroma_idc)
{
    int chroma_height;

    dest_y += (2 * x_offset << pixel_shift) + 2 * y_offset * h->mb_linesize;
    if (chroma_idc == 3 /* yuv444 */) {
        chroma_height     = height;
        chroma_weight_avg = luma_weight_avg;
        chroma_weight_op  = luma_weight_op;
        dest_cb += (2 * x_offset << pixel_shift) + 2 * y_offset * h->mb_linesize;
        dest_cr += (2 * x_offset << pixel_shift) + 2 * y_offset * h->mb_linesize;
    } else if (chroma_idc == 2 /* yuv422 */) {
        chroma_height = height;
        dest_cb      += (x_offset << pixel_shift) + 2 * y_offset * h->mb_uvlinesize;
        dest_cr      += (x_offset << pixel_shift) + 2 * y_offset * h->mb_uvlinesize;
    } else { /* yuv420 */
        chroma_height = height >> 1;
        dest_cb      += (x_offset << pixel_shift) + y_offset * h->mb_uvlinesize;
        dest_cr      += (x_offset << pixel_shift) + y_offset * h->mb_uvlinesize;
    }
    x_offset += 8 * h->mb_x;
    y_offset += 8 * (h->mb_y >> MB_FIELD(h));

    if (list0 && list1) {
        /* don't optimize for luma-only case, since B-frames usually
         * use implicit weights => chroma too. */
        uint8_t *tmp_cb = h->bipred_scratchpad;
        uint8_t *tmp_cr = h->bipred_scratchpad + (16 << pixel_shift);
        uint8_t *tmp_y  = h->bipred_scratchpad + 16 * h->mb_uvlinesize;
        int refn0       = h->ref_cache[0][scan8[n]];
        int refn1       = h->ref_cache[1][scan8[n]];

        mc_dir_part(h, &h->ref_list[0][refn0], n, square, height, delta, 0,
                    dest_y, dest_cb, dest_cr,
                    x_offset, y_offset, qpix_put, chroma_put,
                    pixel_shift, chroma_idc);
        mc_dir_part(h, &h->ref_list[1][refn1], n, square, height, delta, 1,
                    tmp_y, tmp_cb, tmp_cr,
                    x_offset, y_offset, qpix_put, chroma_put,
                    pixel_shift, chroma_idc);

        if (h->use_weight == 2) {
            int weight0 = h->implicit_weight[refn0][refn1][h->mb_y & 1];
            int weight1 = 64 - weight0;
            luma_weight_avg(dest_y, tmp_y, h->mb_linesize,
                            height, 5, weight0, weight1, 0);
            chroma_weight_avg(dest_cb, tmp_cb, h->mb_uvlinesize,
                              chroma_height, 5, weight0, weight1, 0);
            chroma_weight_avg(dest_cr, tmp_cr, h->mb_uvlinesize,
                              chroma_height, 5, weight0, weight1, 0);
        } else {
            luma_weight_avg(dest_y, tmp_y, h->mb_linesize, height,
                            h->luma_log2_weight_denom,
                            h->luma_weight[refn0][0][0],
                            h->luma_weight[refn1][1][0],
                            h->luma_weight[refn0][0][1] +
                            h->luma_weight[refn1][1][1]);
            chroma_weight_avg(dest_cb, tmp_cb, h->mb_uvlinesize, chroma_height,
                              h->chroma_log2_weight_denom,
                              h->chroma_weight[refn0][0][0][0],
                              h->chroma_weight[refn1][1][0][0],
                              h->chroma_weight[refn0][0][0][1] +
                              h->chroma_weight[refn1][1][0][1]);
            chroma_weight_avg(dest_cr, tmp_cr, h->mb_uvlinesize, chroma_height,
                              h->chroma_log2_weight_denom,
                              h->chroma_weight[refn0][0][1][0],
                              h->chroma_weight[refn1][1][1][0],
                              h->chroma_weight[refn0][0][1][1] +
                              h->chroma_weight[refn1][1][1][1]);
        }
    } else {
        int list     = list1 ? 1 : 0;
        int refn     = h->ref_cache[list][scan8[n]];
        Picture *ref = &h->ref_list[list][refn];
        mc_dir_part(h, ref, n, square, height, delta, list,
                    dest_y, dest_cb, dest_cr, x_offset, y_offset,
                    qpix_put, chroma_put, pixel_shift, chroma_idc);

        luma_weight_op(dest_y, h->mb_linesize, height,
                       h->luma_log2_weight_denom,
                       h->luma_weight[refn][list][0],
                       h->luma_weight[refn][list][1]);
        if (h->use_weight_chroma) {
            chroma_weight_op(dest_cb, h->mb_uvlinesize, chroma_height,
                             h->chroma_log2_weight_denom,
                             h->chroma_weight[refn][list][0][0],
                             h->chroma_weight[refn][list][0][1]);
            chroma_weight_op(dest_cr, h->mb_uvlinesize, chroma_height,
                             h->chroma_log2_weight_denom,
                             h->chroma_weight[refn][list][1][0],
                             h->chroma_weight[refn][list][1][1]);
        }
    }
}
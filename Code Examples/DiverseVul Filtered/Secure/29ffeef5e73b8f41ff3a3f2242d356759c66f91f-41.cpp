static av_always_inline void mc_part_std(H264Context *h, int n, int square,
                                         int height, int delta,
                                         uint8_t *dest_y, uint8_t *dest_cb,
                                         uint8_t *dest_cr,
                                         int x_offset, int y_offset,
                                         qpel_mc_func *qpix_put,
                                         h264_chroma_mc_func chroma_put,
                                         qpel_mc_func *qpix_avg,
                                         h264_chroma_mc_func chroma_avg,
                                         int list0, int list1,
                                         int pixel_shift, int chroma_idc)
{
    qpel_mc_func *qpix_op         = qpix_put;
    h264_chroma_mc_func chroma_op = chroma_put;

    dest_y += (2 * x_offset << pixel_shift) + 2 * y_offset * h->mb_linesize;
    if (chroma_idc == 3 /* yuv444 */) {
        dest_cb += (2 * x_offset << pixel_shift) + 2 * y_offset * h->mb_linesize;
        dest_cr += (2 * x_offset << pixel_shift) + 2 * y_offset * h->mb_linesize;
    } else if (chroma_idc == 2 /* yuv422 */) {
        dest_cb += (x_offset << pixel_shift) + 2 * y_offset * h->mb_uvlinesize;
        dest_cr += (x_offset << pixel_shift) + 2 * y_offset * h->mb_uvlinesize;
    } else { /* yuv420 */
        dest_cb += (x_offset << pixel_shift) + y_offset * h->mb_uvlinesize;
        dest_cr += (x_offset << pixel_shift) + y_offset * h->mb_uvlinesize;
    }
    x_offset += 8 * h->mb_x;
    y_offset += 8 * (h->mb_y >> MB_FIELD(h));

    if (list0) {
        Picture *ref = &h->ref_list[0][h->ref_cache[0][scan8[n]]];
        mc_dir_part(h, ref, n, square, height, delta, 0,
                    dest_y, dest_cb, dest_cr, x_offset, y_offset,
                    qpix_op, chroma_op, pixel_shift, chroma_idc);

        qpix_op   = qpix_avg;
        chroma_op = chroma_avg;
    }

    if (list1) {
        Picture *ref = &h->ref_list[1][h->ref_cache[1][scan8[n]]];
        mc_dir_part(h, ref, n, square, height, delta, 1,
                    dest_y, dest_cb, dest_cr, x_offset, y_offset,
                    qpix_op, chroma_op, pixel_shift, chroma_idc);
    }
}
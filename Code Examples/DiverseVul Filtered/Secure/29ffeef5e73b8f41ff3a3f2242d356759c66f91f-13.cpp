static void predict_field_decoding_flag(H264Context *h)
{
    const int mb_xy = h->mb_x + h->mb_y * h->mb_stride;
    int mb_type     = (h->slice_table[mb_xy - 1] == h->slice_num) ?
                      h->cur_pic.mb_type[mb_xy - 1] :
                      (h->slice_table[mb_xy - h->mb_stride] == h->slice_num) ?
                      h->cur_pic.mb_type[mb_xy - h->mb_stride] : 0;
    h->mb_mbaff     = h->mb_field_decoding_flag = IS_INTERLACED(mb_type) ? 1 : 0;
}
static void implicit_weight_table(H264Context *h, int field)
{
    int ref0, ref1, i, cur_poc, ref_start, ref_count0, ref_count1;

    for (i = 0; i < 2; i++) {
        h->luma_weight_flag[i]   = 0;
        h->chroma_weight_flag[i] = 0;
    }

    if (field < 0) {
        if (h->picture_structure == PICT_FRAME) {
            cur_poc = h->cur_pic_ptr->poc;
        } else {
            cur_poc = h->cur_pic_ptr->field_poc[h->picture_structure - 1];
        }
        if (h->ref_count[0] == 1 && h->ref_count[1] == 1 && !FRAME_MBAFF(h) &&
            h->ref_list[0][0].poc + h->ref_list[1][0].poc == 2 * cur_poc) {
            h->use_weight        = 0;
            h->use_weight_chroma = 0;
            return;
        }
        ref_start  = 0;
        ref_count0 = h->ref_count[0];
        ref_count1 = h->ref_count[1];
    } else {
        cur_poc    = h->cur_pic_ptr->field_poc[field];
        ref_start  = 16;
        ref_count0 = 16 + 2 * h->ref_count[0];
        ref_count1 = 16 + 2 * h->ref_count[1];
    }

    h->use_weight               = 2;
    h->use_weight_chroma        = 2;
    h->luma_log2_weight_denom   = 5;
    h->chroma_log2_weight_denom = 5;

    for (ref0 = ref_start; ref0 < ref_count0; ref0++) {
        int poc0 = h->ref_list[0][ref0].poc;
        for (ref1 = ref_start; ref1 < ref_count1; ref1++) {
            int w = 32;
            if (!h->ref_list[0][ref0].long_ref && !h->ref_list[1][ref1].long_ref) {
                int poc1 = h->ref_list[1][ref1].poc;
                int td   = av_clip(poc1 - poc0, -128, 127);
                if (td) {
                    int tb = av_clip(cur_poc - poc0, -128, 127);
                    int tx = (16384 + (FFABS(td) >> 1)) / td;
                    int dist_scale_factor = (tb * tx + 32) >> 8;
                    if (dist_scale_factor >= -64 && dist_scale_factor <= 128)
                        w = 64 - dist_scale_factor;
                }
            }
            if (field < 0) {
                h->implicit_weight[ref0][ref1][0] =
                h->implicit_weight[ref0][ref1][1] = w;
            } else {
                h->implicit_weight[ref0][ref1][field] = w;
            }
        }
    }
}
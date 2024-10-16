int ff_set_ref_count(H264Context *h)
{
    int ref_count[2], list_count;
    int num_ref_idx_active_override_flag;

    // set defaults, might be overridden a few lines later
    ref_count[0] = h->pps.ref_count[0];
    ref_count[1] = h->pps.ref_count[1];

    if (h->slice_type_nos != AV_PICTURE_TYPE_I) {
        unsigned max[2];
        max[0] = max[1] = h->picture_structure == PICT_FRAME ? 15 : 31;

        if (h->slice_type_nos == AV_PICTURE_TYPE_B)
            h->direct_spatial_mv_pred = get_bits1(&h->gb);
        num_ref_idx_active_override_flag = get_bits1(&h->gb);

        if (num_ref_idx_active_override_flag) {
            ref_count[0] = get_ue_golomb(&h->gb) + 1;
            if (h->slice_type_nos == AV_PICTURE_TYPE_B) {
                ref_count[1] = get_ue_golomb(&h->gb) + 1;
            } else
                // full range is spec-ok in this case, even for frames
                ref_count[1] = 1;
        }

        if (ref_count[0]-1 > max[0] || ref_count[1]-1 > max[1]){
            av_log(h->avctx, AV_LOG_ERROR, "reference overflow %u > %u or %u > %u\n", ref_count[0]-1, max[0], ref_count[1]-1, max[1]);
            h->ref_count[0] = h->ref_count[1] = 0;
            h->list_count   = 0;
            return AVERROR_INVALIDDATA;
        }

        if (h->slice_type_nos == AV_PICTURE_TYPE_B)
            list_count = 2;
        else
            list_count = 1;
    } else {
        list_count   = 0;
        ref_count[0] = ref_count[1] = 0;
    }

    if (list_count != h->list_count ||
        ref_count[0] != h->ref_count[0] ||
        ref_count[1] != h->ref_count[1]) {
        h->ref_count[0] = ref_count[0];
        h->ref_count[1] = ref_count[1];
        h->list_count   = list_count;
        return 1;
    }

    return 0;
}
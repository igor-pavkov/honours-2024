int ff_set_ref_count(H264Context *h)
{
    int num_ref_idx_active_override_flag;

    // set defaults, might be overridden a few lines later
    h->ref_count[0] = h->pps.ref_count[0];
    h->ref_count[1] = h->pps.ref_count[1];

    if (h->slice_type_nos != AV_PICTURE_TYPE_I) {
        unsigned max[2];
        max[0] = max[1] = h->picture_structure == PICT_FRAME ? 15 : 31;

        if (h->slice_type_nos == AV_PICTURE_TYPE_B)
            h->direct_spatial_mv_pred = get_bits1(&h->gb);
        num_ref_idx_active_override_flag = get_bits1(&h->gb);

        if (num_ref_idx_active_override_flag) {
            h->ref_count[0] = get_ue_golomb(&h->gb) + 1;
            if (h->slice_type_nos == AV_PICTURE_TYPE_B) {
                h->ref_count[1] = get_ue_golomb(&h->gb) + 1;
            } else
                // full range is spec-ok in this case, even for frames
                h->ref_count[1] = 1;
        }

        if (h->ref_count[0]-1 > max[0] || h->ref_count[1]-1 > max[1]){
            av_log(h->avctx, AV_LOG_ERROR, "reference overflow %u > %u or %u > %u\n", h->ref_count[0]-1, max[0], h->ref_count[1]-1, max[1]);
            h->ref_count[0] = h->ref_count[1] = 0;
            return AVERROR_INVALIDDATA;
        }

        if (h->slice_type_nos == AV_PICTURE_TYPE_B)
            h->list_count = 2;
        else
            h->list_count = 1;
    } else {
        h->list_count   = 0;
        h->ref_count[0] = h->ref_count[1] = 0;
    }

    return 0;
}
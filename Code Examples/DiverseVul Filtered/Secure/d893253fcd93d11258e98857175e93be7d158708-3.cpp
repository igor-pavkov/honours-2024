static void copy_fields(FFV1Context *fsdst, FFV1Context *fssrc, FFV1Context *fsrc)
{
    fsdst->version             = fsrc->version;
    fsdst->micro_version       = fsrc->micro_version;
    fsdst->chroma_planes       = fsrc->chroma_planes;
    fsdst->chroma_h_shift      = fsrc->chroma_h_shift;
    fsdst->chroma_v_shift      = fsrc->chroma_v_shift;
    fsdst->transparency        = fsrc->transparency;
    fsdst->plane_count         = fsrc->plane_count;
    fsdst->ac                  = fsrc->ac;
    fsdst->colorspace          = fsrc->colorspace;

    fsdst->ec                  = fsrc->ec;
    fsdst->intra               = fsrc->intra;
    fsdst->slice_damaged       = fssrc->slice_damaged;
    fsdst->key_frame_ok        = fsrc->key_frame_ok;

    fsdst->bits_per_raw_sample = fsrc->bits_per_raw_sample;
    fsdst->packed_at_lsb       = fsrc->packed_at_lsb;
    fsdst->slice_count         = fsrc->slice_count;
    if (fsrc->version<3){
        fsdst->slice_x             = fssrc->slice_x;
        fsdst->slice_y             = fssrc->slice_y;
        fsdst->slice_width         = fssrc->slice_width;
        fsdst->slice_height        = fssrc->slice_height;
    }
}
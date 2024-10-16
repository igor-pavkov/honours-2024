static void clone_tables(H264Context *dst, H264Context *src, int i)
{
    dst->intra4x4_pred_mode     = src->intra4x4_pred_mode + i * 8 * 2 * src->mb_stride;
    dst->non_zero_count         = src->non_zero_count;
    dst->slice_table            = src->slice_table;
    dst->cbp_table              = src->cbp_table;
    dst->mb2b_xy                = src->mb2b_xy;
    dst->mb2br_xy               = src->mb2br_xy;
    dst->chroma_pred_mode_table = src->chroma_pred_mode_table;
    dst->mvd_table[0]           = src->mvd_table[0] + i * 8 * 2 * src->mb_stride;
    dst->mvd_table[1]           = src->mvd_table[1] + i * 8 * 2 * src->mb_stride;
    dst->direct_table           = src->direct_table;
    dst->list_counts            = src->list_counts;
    dst->DPB                    = src->DPB;
    dst->cur_pic_ptr            = src->cur_pic_ptr;
    dst->cur_pic                = src->cur_pic;
    dst->bipred_scratchpad      = NULL;
    dst->edge_emu_buffer        = NULL;
    dst->me.scratchpad          = NULL;
    ff_h264_pred_init(&dst->hpc, src->avctx->codec_id, src->sps.bit_depth_luma,
                      src->sps.chroma_format_idc);
}
static int clone_slice(H264Context *dst, H264Context *src)
{
    memcpy(dst->block_offset, src->block_offset, sizeof(dst->block_offset));
    dst->cur_pic_ptr = src->cur_pic_ptr;
    dst->cur_pic     = src->cur_pic;
    dst->linesize    = src->linesize;
    dst->uvlinesize  = src->uvlinesize;
    dst->first_field = src->first_field;

    dst->prev_poc_msb          = src->prev_poc_msb;
    dst->prev_poc_lsb          = src->prev_poc_lsb;
    dst->prev_frame_num_offset = src->prev_frame_num_offset;
    dst->prev_frame_num        = src->prev_frame_num;
    dst->short_ref_count       = src->short_ref_count;

    memcpy(dst->short_ref,        src->short_ref,        sizeof(dst->short_ref));
    memcpy(dst->long_ref,         src->long_ref,         sizeof(dst->long_ref));
    memcpy(dst->default_ref_list, src->default_ref_list, sizeof(dst->default_ref_list));

    memcpy(dst->dequant4_coeff,   src->dequant4_coeff,   sizeof(src->dequant4_coeff));
    memcpy(dst->dequant8_coeff,   src->dequant8_coeff,   sizeof(src->dequant8_coeff));

    return 0;
}
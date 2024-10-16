static void iscsi_xcopy_populate_header(unsigned char *buf, int list_id, int str,
                                        int list_id_usage, int prio,
                                        int tgt_desc_len,
                                        int seg_desc_len, int inline_data_len)
{
    buf[0] = list_id;
    buf[1] = ((str & 1) << 5) | ((list_id_usage & 3) << 3) | (prio & 7);
    buf[2] = (tgt_desc_len >> 8) & 0xFF;
    buf[3] = tgt_desc_len & 0xFF;
    buf[8] = (seg_desc_len >> 24) & 0xFF;
    buf[9] = (seg_desc_len >> 16) & 0xFF;
    buf[10] = (seg_desc_len >> 8) & 0xFF;
    buf[11] = seg_desc_len & 0xFF;
    buf[12] = (inline_data_len >> 24) & 0xFF;
    buf[13] = (inline_data_len >> 16) & 0xFF;
    buf[14] = (inline_data_len >> 8) & 0xFF;
    buf[15] = inline_data_len & 0xFF;
}
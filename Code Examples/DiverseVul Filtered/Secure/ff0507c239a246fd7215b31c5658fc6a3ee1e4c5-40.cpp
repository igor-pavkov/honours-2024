static void iscsi_xcopy_data(struct iscsi_data *data,
                             IscsiLun *src, int64_t src_lba,
                             IscsiLun *dst, int64_t dst_lba,
                             uint16_t num_blocks)
{
    uint8_t *buf;
    const int src_offset = XCOPY_DESC_OFFSET;
    const int dst_offset = XCOPY_DESC_OFFSET + IDENT_DESCR_TGT_DESCR_SIZE;
    const int seg_offset = dst_offset + IDENT_DESCR_TGT_DESCR_SIZE;

    data->size = XCOPY_DESC_OFFSET +
                 IDENT_DESCR_TGT_DESCR_SIZE * 2 +
                 XCOPY_BLK2BLK_SEG_DESC_SIZE;
    data->data = g_malloc0(data->size);
    buf = data->data;

    /* Initialise the parameter list header */
    iscsi_xcopy_populate_header(buf, 1, 0, 2 /* LIST_ID_USAGE_DISCARD */,
                                0, 2 * IDENT_DESCR_TGT_DESCR_SIZE,
                                XCOPY_BLK2BLK_SEG_DESC_SIZE,
                                0);

    /* Initialise CSCD list with one src + one dst descriptor */
    iscsi_populate_target_desc(&buf[src_offset], src);
    iscsi_populate_target_desc(&buf[dst_offset], dst);

    /* Initialise one segment descriptor */
    iscsi_xcopy_populate_desc(&buf[seg_offset], 0, 0, 0, 1, num_blocks,
                              src_lba, dst_lba);
}
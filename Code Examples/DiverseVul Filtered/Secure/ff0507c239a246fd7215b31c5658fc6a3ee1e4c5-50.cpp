static void iscsi_xcopy_populate_desc(uint8_t *desc, int dc, int cat,
                                      int src_index, int dst_index, int num_blks,
                                      uint64_t src_lba, uint64_t dst_lba)
{
    iscsi_xcopy_desc_hdr(desc, dc, cat, src_index, dst_index);

    /* The caller should verify the request size */
    assert(num_blks < 65536);
    desc[10] = (num_blks >> 8) & 0xFF;
    desc[11] = num_blks & 0xFF;
    desc[12] = (src_lba >> 56) & 0xFF;
    desc[13] = (src_lba >> 48) & 0xFF;
    desc[14] = (src_lba >> 40) & 0xFF;
    desc[15] = (src_lba >> 32) & 0xFF;
    desc[16] = (src_lba >> 24) & 0xFF;
    desc[17] = (src_lba >> 16) & 0xFF;
    desc[18] = (src_lba >> 8) & 0xFF;
    desc[19] = src_lba & 0xFF;
    desc[20] = (dst_lba >> 56) & 0xFF;
    desc[21] = (dst_lba >> 48) & 0xFF;
    desc[22] = (dst_lba >> 40) & 0xFF;
    desc[23] = (dst_lba >> 32) & 0xFF;
    desc[24] = (dst_lba >> 24) & 0xFF;
    desc[25] = (dst_lba >> 16) & 0xFF;
    desc[26] = (dst_lba >> 8) & 0xFF;
    desc[27] = dst_lba & 0xFF;
}
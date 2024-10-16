static void iscsi_xcopy_desc_hdr(uint8_t *hdr, int dc, int cat, int src_index,
                                 int dst_index)
{
    hdr[0] = 0x02; /* BLK_TO_BLK_SEG_DESCR */
    hdr[1] = ((dc << 1) | cat) & 0xFF;
    hdr[2] = (XCOPY_BLK2BLK_SEG_DESC_SIZE >> 8) & 0xFF;
    /* don't account for the first 4 bytes in descriptor header*/
    hdr[3] = (XCOPY_BLK2BLK_SEG_DESC_SIZE - 4 /* SEG_DESC_SRC_INDEX_OFFSET */) & 0xFF;
    hdr[4] = (src_index >> 8) & 0xFF;
    hdr[5] = src_index & 0xFF;
    hdr[6] = (dst_index >> 8) & 0xFF;
    hdr[7] = dst_index & 0xFF;
}
static int load_xbzrle(QEMUFile *f, ram_addr_t addr, void *host)
{
    unsigned int xh_len;
    int xh_flags;

    if (!xbzrle_decoded_buf) {
        xbzrle_decoded_buf = g_malloc(TARGET_PAGE_SIZE);
    }

    /* extract RLE header */
    xh_flags = qemu_get_byte(f);
    xh_len = qemu_get_be16(f);

    if (xh_flags != ENCODING_FLAG_XBZRLE) {
        error_report("Failed to load XBZRLE page - wrong compression!");
        return -1;
    }

    if (xh_len > TARGET_PAGE_SIZE) {
        error_report("Failed to load XBZRLE page - len overflow!");
        return -1;
    }
    /* load data and decode */
    qemu_get_buffer(f, xbzrle_decoded_buf, xh_len);

    /* decode RLE */
    if (xbzrle_decode_buffer(xbzrle_decoded_buf, xh_len, host,
                             TARGET_PAGE_SIZE) == -1) {
        error_report("Failed to load XBZRLE page - decode error!");
        return -1;
    }

    return 0;
}
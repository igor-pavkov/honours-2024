uint32_t vhdx_checksum_calc(uint32_t crc, uint8_t *buf, size_t size,
                            int crc_offset)
{
    uint32_t crc_new;
    uint32_t crc_orig;
    assert(buf != NULL);

    if (crc_offset > 0) {
        memcpy(&crc_orig, buf + crc_offset, sizeof(crc_orig));
        memset(buf + crc_offset, 0, sizeof(crc_orig));
    }

    crc_new = crc32c(crc, buf, size);
    if (crc_offset > 0) {
        memcpy(buf + crc_offset, &crc_orig, sizeof(crc_orig));
    }

    return crc_new;
}
bool vhdx_checksum_is_valid(uint8_t *buf, size_t size, int crc_offset)
{
    uint32_t crc_orig;
    uint32_t crc;

    assert(buf != NULL);
    assert(size > (crc_offset + 4));

    memcpy(&crc_orig, buf + crc_offset, sizeof(crc_orig));
    crc_orig = le32_to_cpu(crc_orig);

    crc = vhdx_checksum_calc(0xffffffff, buf, size, crc_offset);

    return crc == crc_orig;
}
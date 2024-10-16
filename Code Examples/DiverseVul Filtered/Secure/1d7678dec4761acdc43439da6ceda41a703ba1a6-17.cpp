uint32_t vhdx_update_checksum(uint8_t *buf, size_t size, int crc_offset)
{
    uint32_t crc;

    assert(buf != NULL);
    assert(size > (crc_offset + sizeof(crc)));

    memset(buf + crc_offset, 0, sizeof(crc));
    crc =  crc32c(0xffffffff, buf, size);
    memcpy(buf + crc_offset, &crc, sizeof(crc));

    return crc;
}
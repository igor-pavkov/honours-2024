static void write_unicode(bytearray_t * bplist, uint16_t * val, uint64_t size)
{
    uint64_t i = 0;
    uint64_t size2 = size * sizeof(uint16_t);
    uint8_t *buff = (uint8_t *) malloc(size2);
    memcpy(buff, val, size2);
    for (i = 0; i < size; i++)
        byte_convert(buff + i * sizeof(uint16_t), sizeof(uint16_t));
    write_raw_data(bplist, BPLIST_UNICODE, buff, size);
    free(buff);
}
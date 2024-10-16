static void write_date(bytearray_t * bplist, double val)
{
    uint64_t size = 8;			//dates always use 8 bytes
    uint8_t *buff = (uint8_t *) malloc(sizeof(uint8_t) + size);
    buff[0] = BPLIST_DATE | Log2(size);
    memcpy(buff + 1, &val, size);
    float_byte_convert(buff + 1, size);
    byte_array_append(bplist, buff, sizeof(uint8_t) + size);
    free(buff);
}
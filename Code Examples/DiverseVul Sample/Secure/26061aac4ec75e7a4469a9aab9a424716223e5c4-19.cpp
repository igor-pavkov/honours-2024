static void write_uint(bytearray_t * bplist, uint64_t val)
{
    uint64_t size = 16;
    uint8_t *buff = NULL;

    buff = (uint8_t *) malloc(sizeof(uint8_t) + size);
    buff[0] = BPLIST_UINT | 4;
    memset(buff + 1, '\0', 8);
    memcpy(buff + 9, &val, 8);
    byte_convert(buff + 9, 8);
    byte_array_append(bplist, buff, sizeof(uint8_t) + size);
    free(buff);
}
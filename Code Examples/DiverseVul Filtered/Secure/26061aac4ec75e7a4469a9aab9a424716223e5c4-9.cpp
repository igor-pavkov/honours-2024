static void write_raw_data(bytearray_t * bplist, uint8_t mark, uint8_t * val, uint64_t size)
{
    uint8_t *buff = NULL;
    uint8_t marker = mark | (size < 15 ? size : 0xf);
    byte_array_append(bplist, &marker, sizeof(uint8_t));
    if (size >= 15)
    {
        bytearray_t *int_buff = byte_array_new();
        write_int(int_buff, size);
        byte_array_append(bplist, int_buff->data, int_buff->len);
        byte_array_free(int_buff);
    }
    //stupid unicode buffer length
    if (BPLIST_UNICODE==mark) size *= 2;
    buff = (uint8_t *) malloc(size);
    memcpy(buff, val, size);
    byte_array_append(bplist, buff, size);
    free(buff);
}
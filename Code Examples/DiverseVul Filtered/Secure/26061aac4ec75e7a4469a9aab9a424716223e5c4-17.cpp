static void write_int(bytearray_t * bplist, uint64_t val)
{
    uint64_t size = get_needed_bytes(val);
    uint8_t *buff = NULL;
    //do not write 3bytes int node
    if (size == 3)
        size++;

#ifdef __BIG_ENDIAN__
    val = val << ((sizeof(uint64_t) - size) * 8);
#endif

    buff = (uint8_t *) malloc(sizeof(uint8_t) + size);
    buff[0] = BPLIST_UINT | Log2(size);
    memcpy(buff + 1, &val, size);
    byte_convert(buff + 1, size);
    byte_array_append(bplist, buff, sizeof(uint8_t) + size);
    free(buff);
}
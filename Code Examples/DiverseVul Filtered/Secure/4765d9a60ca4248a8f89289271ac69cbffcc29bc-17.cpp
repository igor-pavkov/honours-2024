static void write_uint(bytearray_t * bplist, uint64_t val)
{
    uint8_t sz = BPLIST_UINT | 4;
    uint64_t zero = 0;

    val = be64toh(val);
    byte_array_append(bplist, &sz, 1);
    byte_array_append(bplist, &zero, sizeof(uint64_t));
    byte_array_append(bplist, &val, sizeof(uint64_t));
}
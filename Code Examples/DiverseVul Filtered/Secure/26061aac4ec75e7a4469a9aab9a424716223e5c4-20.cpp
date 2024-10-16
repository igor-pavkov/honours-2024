static void write_real(bytearray_t * bplist, double val)
{
    uint64_t size = get_real_bytes(val);	//cheat to know used space
    uint8_t *buff = (uint8_t *) malloc(sizeof(uint8_t) + size);
    buff[0] = BPLIST_REAL | Log2(size);
    if (size == sizeof(double))
    {
        memcpy(buff + 1, &val, size);
    }
    else if (size == sizeof(float))
    {
        float tmpval = (float) val;
        memcpy(buff + 1, &tmpval, size);
    }
    float_byte_convert(buff + 1, size);
    byte_array_append(bplist, buff, sizeof(uint8_t) + size);
    free(buff);
}
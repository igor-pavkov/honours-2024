static uint32_t uint24_from_be(union plist_uint_ptr buf)
{
    union plist_uint_ptr tmp;
    uint32_t ret = 0;

    tmp.src = &ret;

    memcpy(tmp.u8ptr + 1, buf.u8ptr, 3 * sizeof(char));

    byte_convert(tmp.u8ptr, sizeof(uint32_t));
    return ret;
}
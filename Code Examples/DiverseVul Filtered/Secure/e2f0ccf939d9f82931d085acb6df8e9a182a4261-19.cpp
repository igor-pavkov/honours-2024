bool pb_decode_bool(pb_istream_t *stream, bool *dest)
{
    uint32_t value;
    if (!pb_decode_varint32(stream, &value))
        return false;

    *(bool*)dest = (value != 0);
    return true;
}
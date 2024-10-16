bool pb_decode_svarint(pb_istream_t *stream, int64_t *dest)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    if (value & 1)
        *dest = (int64_t)(~(value >> 1));
    else
        *dest = (int64_t)(value >> 1);
    
    return true;
}
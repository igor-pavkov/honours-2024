bool checkreturn pb_skip_string(pb_istream_t *stream)
{
    uint32_t length;
    if (!pb_decode_varint32(stream, &length))
        return false;
    
    if ((size_t)length != length)
    {
        PB_RETURN_ERROR(stream, "size too large");
    }

    return pb_read(stream, NULL, (size_t)length);
}
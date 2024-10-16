static bool checkreturn pb_readbyte(pb_istream_t *stream, uint8_t *buf)
{
    if (stream->bytes_left == 0)
        PB_RETURN_ERROR(stream, "end-of-stream");

#ifndef PB_BUFFER_ONLY
    if (!stream->callback(stream, buf, 1))
        PB_RETURN_ERROR(stream, "io error");
#else
    *buf = *(uint8_t*)stream->state;
    stream->state = (uint8_t*)stream->state + 1;
#endif

    stream->bytes_left--;
    
    return true;    
}
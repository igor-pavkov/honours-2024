static bool checkreturn buf_read(pb_istream_t *stream, uint8_t *buf, size_t count)
{
    uint8_t *source = (uint8_t*)stream->state;
    stream->state = source + count;
    
    if (buf != NULL)
    {
        while (count--)
            *buf++ = *source++;
    }
    
    return true;
}
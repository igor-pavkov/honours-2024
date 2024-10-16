void pb_close_string_substream(pb_istream_t *stream, pb_istream_t *substream)
{
    stream->state = substream->state;

#ifndef PB_NO_ERRMSG
    stream->errmsg = substream->errmsg;
#endif
}
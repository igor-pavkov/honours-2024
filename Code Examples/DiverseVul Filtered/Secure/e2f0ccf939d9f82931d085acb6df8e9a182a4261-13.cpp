bool checkreturn pb_decode_ex(pb_istream_t *stream, const pb_msgdesc_t *fields, void *dest_struct, unsigned int flags)
{
    bool status;

    if ((flags & PB_DECODE_DELIMITED) == 0)
    {
      status = pb_decode_inner(stream, fields, dest_struct, flags);
    }
    else
    {
      pb_istream_t substream;
      if (!pb_make_string_substream(stream, &substream))
        return false;

      status = pb_decode_inner(&substream, fields, dest_struct, flags);

      if (!pb_close_string_substream(stream, &substream))
        return false;
    }
    
#ifdef PB_ENABLE_MALLOC
    if (!status)
        pb_release(fields, dest_struct);
#endif
    
    return status;
}
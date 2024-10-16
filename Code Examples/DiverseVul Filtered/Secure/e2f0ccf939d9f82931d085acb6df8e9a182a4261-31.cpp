bool checkreturn pb_decode(pb_istream_t *stream, const pb_msgdesc_t *fields, void *dest_struct)
{
    bool status;

    status = pb_decode_inner(stream, fields, dest_struct, 0);

#ifdef PB_ENABLE_MALLOC
    if (!status)
        pb_release(fields, dest_struct);
#endif

    return status;
}
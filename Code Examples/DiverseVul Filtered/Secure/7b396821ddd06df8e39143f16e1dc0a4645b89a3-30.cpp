static bool checkreturn pb_dec_fixed32(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    UNUSED(field);
    return pb_decode_fixed32(stream, dest);
}
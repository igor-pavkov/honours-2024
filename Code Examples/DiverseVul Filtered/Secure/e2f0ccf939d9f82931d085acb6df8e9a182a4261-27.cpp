static bool checkreturn pb_dec_bool(pb_istream_t *stream, const pb_field_iter_t *field)
{
    return pb_decode_bool(stream, (bool*)field->pData);
}
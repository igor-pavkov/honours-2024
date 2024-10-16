static bool checkreturn pb_dec_fixed(pb_istream_t *stream, const pb_field_iter_t *field)
{
#ifdef PB_CONVERT_DOUBLE_FLOAT
    if (field->data_size == sizeof(float) && PB_LTYPE(field->type) == PB_LTYPE_FIXED64)
    {
        return pb_decode_double_as_float(stream, (float*)field->pData);
    }
#endif

    if (field->data_size == sizeof(uint32_t))
    {
        return pb_decode_fixed32(stream, field->pData);
    }
#ifndef PB_WITHOUT_64BIT
    else if (field->data_size == sizeof(uint64_t))
    {
        return pb_decode_fixed64(stream, field->pData);
    }
#endif
    else
    {
        PB_RETURN_ERROR(stream, "invalid data_size");
    }
}
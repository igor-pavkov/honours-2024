static bool checkreturn pb_dec_svarint(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    int64_t value;
    if (!pb_decode_svarint(stream, &value))
        return false;
    
    switch (field->data_size)
    {
        case 4: *(int32_t*)dest = (int32_t)value; break;
        case 8: *(int64_t*)dest = value; break;
        default: PB_RETURN_ERROR(stream, "invalid data_size");
    }
    
    return true;
}
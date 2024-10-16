static bool checkreturn pb_dec_varint(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    switch (field->data_size)
    {
        case 1: *(int8_t*)dest = (int8_t)value; break;
        case 2: *(int16_t*)dest = (int16_t)value; break;
        case 4: *(int32_t*)dest = (int32_t)value; break;
        case 8: *(int64_t*)dest = (int64_t)value; break;
        default: PB_RETURN_ERROR(stream, "invalid data_size");
    }
    
    return true;
}
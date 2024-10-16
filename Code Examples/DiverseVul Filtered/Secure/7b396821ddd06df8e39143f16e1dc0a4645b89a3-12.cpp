static bool checkreturn pb_dec_uvarint(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    uint64_t value;
    if (!pb_decode_varint(stream, &value))
        return false;
    
    switch (field->data_size)
    {
        case 4: *(uint32_t*)dest = (uint32_t)value; break;
        case 8: *(uint64_t*)dest = value; break;
        default: PB_RETURN_ERROR(stream, "invalid data_size");
    }
    
    return true;
}
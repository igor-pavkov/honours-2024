static bool checkreturn pb_dec_string(pb_istream_t *stream, const pb_field_t *field, void *dest)
{
    uint32_t size;
    size_t alloc_size;
    bool status;
    if (!pb_decode_varint32(stream, &size))
        return false;
    
    /* Space for null terminator */
    alloc_size = size + 1;
    
    if (alloc_size < size)
        PB_RETURN_ERROR(stream, "size too large");
    
    if (PB_ATYPE(field->type) == PB_ATYPE_POINTER)
    {
#ifndef PB_ENABLE_MALLOC
        PB_RETURN_ERROR(stream, "no malloc support");
#else
        if (!allocate_field(stream, dest, alloc_size, 1))
            return false;
        dest = *(void**)dest;
#endif
    }
    else
    {
        if (alloc_size > field->data_size)
            PB_RETURN_ERROR(stream, "string overflow");
    }
    
    status = pb_read(stream, (uint8_t*)dest, size);
    *((uint8_t*)dest + size) = 0;
    return status;
}
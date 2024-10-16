static bool checkreturn pb_dec_bytes(pb_istream_t *stream, const pb_field_iter_t *field)
{
    uint32_t size;
    size_t alloc_size;
    pb_bytes_array_t *dest;
    
    if (!pb_decode_varint32(stream, &size))
        return false;
    
    if (size > PB_SIZE_MAX)
        PB_RETURN_ERROR(stream, "bytes overflow");
    
    alloc_size = PB_BYTES_ARRAY_T_ALLOCSIZE(size);
    if (size > alloc_size)
        PB_RETURN_ERROR(stream, "size too large");
    
    if (PB_ATYPE(field->type) == PB_ATYPE_POINTER)
    {
#ifndef PB_ENABLE_MALLOC
        PB_RETURN_ERROR(stream, "no malloc support");
#else
        if (stream->bytes_left < size)
            PB_RETURN_ERROR(stream, "end-of-stream");

        if (!allocate_field(stream, field->pData, alloc_size, 1))
            return false;
        dest = *(pb_bytes_array_t**)field->pData;
#endif
    }
    else
    {
        if (alloc_size > field->data_size)
            PB_RETURN_ERROR(stream, "bytes overflow");
        dest = (pb_bytes_array_t*)field->pData;
    }

    dest->size = (pb_size_t)size;
    return pb_read(stream, dest->bytes, (size_t)size);
}
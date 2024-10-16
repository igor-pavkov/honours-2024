static bool checkreturn pb_dec_string(pb_istream_t *stream, const pb_field_iter_t *field)
{
    uint32_t size;
    size_t alloc_size;
    pb_byte_t *dest = (pb_byte_t*)field->pData;

    if (!pb_decode_varint32(stream, &size))
        return false;

    if (size == (uint32_t)-1)
        PB_RETURN_ERROR(stream, "size too large");

    /* Space for null terminator */
    alloc_size = (size_t)(size + 1);

    if (alloc_size < size)
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
        dest = *(pb_byte_t**)field->pData;
#endif
    }
    else
    {
        if (alloc_size > field->data_size)
            PB_RETURN_ERROR(stream, "string overflow");
    }
    
    dest[size] = 0;

    if (!pb_read(stream, dest, (size_t)size))
        return false;

#ifdef PB_VALIDATE_UTF8
    if (!pb_validate_utf8((const char*)dest))
        PB_RETURN_ERROR(stream, "invalid utf8");
#endif

    return true;
}
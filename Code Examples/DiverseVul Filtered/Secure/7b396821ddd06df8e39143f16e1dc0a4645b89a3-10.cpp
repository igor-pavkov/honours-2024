static bool checkreturn decode_field(pb_istream_t *stream, pb_wire_type_t wire_type, pb_field_iterator_t *iter)
{
    switch (PB_ATYPE(iter->pos->type))
    {
        case PB_ATYPE_STATIC:
            return decode_static_field(stream, wire_type, iter);
        
        case PB_ATYPE_POINTER:
            return decode_pointer_field(stream, wire_type, iter);
        
        case PB_ATYPE_CALLBACK:
            return decode_callback_field(stream, wire_type, iter);
        
        default:
            PB_RETURN_ERROR(stream, "invalid field type");
    }
}
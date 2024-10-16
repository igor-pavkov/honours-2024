static bool checkreturn decode_field(pb_istream_t *stream, pb_wire_type_t wire_type, pb_field_iter_t *field)
{
#ifdef PB_ENABLE_MALLOC
    /* When decoding an oneof field, check if there is old data that must be
     * released first. */
    if (PB_HTYPE(field->type) == PB_HTYPE_ONEOF)
    {
        if (!pb_release_union_field(stream, field))
            return false;
    }
#endif

    switch (PB_ATYPE(field->type))
    {
        case PB_ATYPE_STATIC:
            return decode_static_field(stream, wire_type, field);
        
        case PB_ATYPE_POINTER:
            return decode_pointer_field(stream, wire_type, field);
        
        case PB_ATYPE_CALLBACK:
            return decode_callback_field(stream, wire_type, field);
        
        default:
            PB_RETURN_ERROR(stream, "invalid field type");
    }
}
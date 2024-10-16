static bool checkreturn default_extension_decoder(pb_istream_t *stream,
    pb_extension_t *extension, uint32_t tag, pb_wire_type_t wire_type)
{
    const pb_field_t *field = (const pb_field_t*)extension->type->arg;
    pb_field_iterator_t iter;
    
    if (field->tag != tag)
        return true;
    
    iter_from_extension(&iter, extension);
    return decode_field(stream, wire_type, &iter);
}
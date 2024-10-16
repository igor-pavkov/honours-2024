static bool checkreturn default_extension_decoder(pb_istream_t *stream,
    pb_extension_t *extension, uint32_t tag, pb_wire_type_t wire_type)
{
    pb_field_iter_t iter;

    if (!pb_field_iter_begin_extension(&iter, extension))
        PB_RETURN_ERROR(stream, "invalid extension");

    if (iter.tag != tag || !iter.message)
        return true;

    extension->found = true;
    return decode_field(stream, wire_type, &iter);
}
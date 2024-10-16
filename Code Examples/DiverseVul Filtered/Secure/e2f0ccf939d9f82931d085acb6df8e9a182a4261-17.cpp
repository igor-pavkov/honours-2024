static bool checkreturn decode_callback_field(pb_istream_t *stream, pb_wire_type_t wire_type, pb_field_iter_t *field)
{
    if (!field->descriptor->field_callback)
        return pb_skip_field(stream, wire_type);

    if (wire_type == PB_WT_STRING)
    {
        pb_istream_t substream;
        size_t prev_bytes_left;
        
        if (!pb_make_string_substream(stream, &substream))
            return false;
        
        do
        {
            prev_bytes_left = substream.bytes_left;
            if (!field->descriptor->field_callback(&substream, NULL, field))
                PB_RETURN_ERROR(stream, "callback failed");
        } while (substream.bytes_left > 0 && substream.bytes_left < prev_bytes_left);
        
        if (!pb_close_string_substream(stream, &substream))
            return false;

        return true;
    }
    else
    {
        /* Copy the single scalar value to stack.
         * This is required so that we can limit the stream length,
         * which in turn allows to use same callback for packed and
         * not-packed fields. */
        pb_istream_t substream;
        pb_byte_t buffer[10];
        size_t size = sizeof(buffer);
        
        if (!read_raw_value(stream, wire_type, buffer, &size))
            return false;
        substream = pb_istream_from_buffer(buffer, size);
        
        return field->descriptor->field_callback(&substream, NULL, field);
    }
}
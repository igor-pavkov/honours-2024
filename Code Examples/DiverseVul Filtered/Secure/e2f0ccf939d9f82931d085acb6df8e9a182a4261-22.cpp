static bool pb_message_set_to_defaults(pb_field_iter_t *iter)
{
    pb_istream_t defstream = PB_ISTREAM_EMPTY;
    uint32_t tag = 0;
    pb_wire_type_t wire_type = PB_WT_VARINT;
    bool eof;

    if (iter->descriptor->default_value)
    {
        defstream = pb_istream_from_buffer(iter->descriptor->default_value, (size_t)-1);
        if (!pb_decode_tag(&defstream, &wire_type, &tag, &eof))
            return false;
    }

    do
    {
        if (!pb_field_set_to_default(iter))
            return false;

        if (tag != 0 && iter->tag == tag)
        {
            /* We have a default value for this field in the defstream */
            if (!decode_field(&defstream, wire_type, iter))
                return false;
            if (!pb_decode_tag(&defstream, &wire_type, &tag, &eof))
                return false;

            if (iter->pSize)
                *(bool*)iter->pSize = false;
        }
    } while (pb_field_iter_next(iter));

    return true;
}
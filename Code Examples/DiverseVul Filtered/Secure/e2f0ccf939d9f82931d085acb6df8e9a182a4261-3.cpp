static bool checkreturn read_raw_value(pb_istream_t *stream, pb_wire_type_t wire_type, pb_byte_t *buf, size_t *size)
{
    size_t max_size = *size;
    switch (wire_type)
    {
        case PB_WT_VARINT:
            *size = 0;
            do
            {
                (*size)++;
                if (*size > max_size)
                    PB_RETURN_ERROR(stream, "varint overflow");

                if (!pb_read(stream, buf, 1))
                    return false;
            } while (*buf++ & 0x80);
            return true;
            
        case PB_WT_64BIT:
            *size = 8;
            return pb_read(stream, buf, 8);
        
        case PB_WT_32BIT:
            *size = 4;
            return pb_read(stream, buf, 4);
        
        case PB_WT_STRING:
            /* Calling read_raw_value with a PB_WT_STRING is an error.
             * Explicitly handle this case and fallthrough to default to avoid
             * compiler warnings.
             */

        default: PB_RETURN_ERROR(stream, "invalid wire_type");
    }
}
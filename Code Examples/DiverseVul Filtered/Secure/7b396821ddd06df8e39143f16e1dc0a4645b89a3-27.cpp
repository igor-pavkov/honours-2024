bool checkreturn pb_decode_noinit(pb_istream_t *stream, const pb_field_t fields[], void *dest_struct)
{
    uint8_t fields_seen[(PB_MAX_REQUIRED_FIELDS + 7) / 8] = {0, 0, 0, 0, 0, 0, 0, 0};
    uint32_t extension_range_start = 0;
    pb_field_iterator_t iter;
    
    pb_field_init(&iter, fields, dest_struct);
    
    while (stream->bytes_left)
    {
        uint32_t tag;
        pb_wire_type_t wire_type;
        bool eof;
        
        if (!pb_decode_tag(stream, &wire_type, &tag, &eof))
        {
            if (eof)
                break;
            else
                return false;
        }
        
        if (!pb_field_find(&iter, tag))
        {
            /* No match found, check if it matches an extension. */
            if (tag >= extension_range_start)
            {
                if (!find_extension_field(&iter))
                    extension_range_start = (uint32_t)-1;
                else
                    extension_range_start = iter.pos->tag;
                
                if (tag >= extension_range_start)
                {
                    size_t pos = stream->bytes_left;
                
                    if (!decode_extension(stream, tag, wire_type, &iter))
                        return false;
                    
                    if (pos != stream->bytes_left)
                    {
                        /* The field was handled */
                        continue;                    
                    }
                }
            }
        
            /* No match found, skip data */
            if (!pb_skip_field(stream, wire_type))
                return false;
            continue;
        }
        
        if (PB_HTYPE(iter.pos->type) == PB_HTYPE_REQUIRED
            && iter.required_field_index < PB_MAX_REQUIRED_FIELDS)
        {
            fields_seen[iter.required_field_index >> 3] |= (uint8_t)(1 << (iter.required_field_index & 7));
        }
            
        if (!decode_field(stream, wire_type, &iter))
            return false;
    }
    
    /* Check that all required fields were present. */
    {
        /* First figure out the number of required fields by
         * seeking to the end of the field array. Usually we
         * are already close to end after decoding.
         */
        unsigned req_field_count;
        pb_type_t last_type;
        unsigned i;
        do {
            req_field_count = iter.required_field_index;
            last_type = iter.pos->type;
        } while (pb_field_next(&iter));
        
        /* Fixup if last field was also required. */
        if (PB_HTYPE(last_type) == PB_HTYPE_REQUIRED && iter.pos->tag != 0)
            req_field_count++;
        
        /* Check the whole bytes */
        for (i = 0; i < (req_field_count >> 3); i++)
        {
            if (fields_seen[i] != 0xFF)
                PB_RETURN_ERROR(stream, "missing required field");
        }
        
        /* Check the remaining bits */
        if (fields_seen[req_field_count >> 3] != (0xFF >> (8 - (req_field_count & 7))))
            PB_RETURN_ERROR(stream, "missing required field");
    }
    
    return true;
}
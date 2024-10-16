static bool checkreturn pb_decode_inner(pb_istream_t *stream, const pb_msgdesc_t *fields, void *dest_struct, unsigned int flags)
{
    uint32_t extension_range_start = 0;
    pb_extension_t *extensions = NULL;

    /* 'fixed_count_field' and 'fixed_count_size' track position of a repeated fixed
     * count field. This can only handle _one_ repeated fixed count field that
     * is unpacked and unordered among other (non repeated fixed count) fields.
     */
    pb_size_t fixed_count_field = PB_SIZE_MAX;
    pb_size_t fixed_count_size = 0;
    pb_size_t fixed_count_total_size = 0;

    pb_fields_seen_t fields_seen = {{0, 0}};
    const uint32_t allbits = ~(uint32_t)0;
    pb_field_iter_t iter;

    if (pb_field_iter_begin(&iter, fields, dest_struct))
    {
        if ((flags & PB_DECODE_NOINIT) == 0)
        {
            if (!pb_message_set_to_defaults(&iter))
                PB_RETURN_ERROR(stream, "failed to set defaults");
        }
    }

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

        if (tag == 0)
        {
          if (flags & PB_DECODE_NULLTERMINATED)
          {
            break;
          }
          else
          {
            PB_RETURN_ERROR(stream, "zero tag");
          }
        }

        if (!pb_field_iter_find(&iter, tag) || PB_LTYPE(iter.type) == PB_LTYPE_EXTENSION)
        {
            /* No match found, check if it matches an extension. */
            if (extension_range_start == 0)
            {
                if (pb_field_iter_find_extension(&iter))
                {
                    extensions = *(pb_extension_t* const *)iter.pData;
                    extension_range_start = iter.tag;
                }

                if (!extensions)
                {
                    extension_range_start = (uint32_t)-1;
                }
            }

            if (tag >= extension_range_start)
            {
                size_t pos = stream->bytes_left;

                if (!decode_extension(stream, tag, wire_type, extensions))
                    return false;

                if (pos != stream->bytes_left)
                {
                    /* The field was handled */
                    continue;
                }
            }

            /* No match found, skip data */
            if (!pb_skip_field(stream, wire_type))
                return false;
            continue;
        }

        /* If a repeated fixed count field was found, get size from
         * 'fixed_count_field' as there is no counter contained in the struct.
         */
        if (PB_HTYPE(iter.type) == PB_HTYPE_REPEATED && iter.pSize == &iter.array_size)
        {
            if (fixed_count_field != iter.index) {
                /* If the new fixed count field does not match the previous one,
                 * check that the previous one is NULL or that it finished
                 * receiving all the expected data.
                 */
                if (fixed_count_field != PB_SIZE_MAX &&
                    fixed_count_size != fixed_count_total_size)
                {
                    PB_RETURN_ERROR(stream, "wrong size for fixed count field");
                }

                fixed_count_field = iter.index;
                fixed_count_size = 0;
                fixed_count_total_size = iter.array_size;
            }

            iter.pSize = &fixed_count_size;
        }

        if (PB_HTYPE(iter.type) == PB_HTYPE_REQUIRED
            && iter.required_field_index < PB_MAX_REQUIRED_FIELDS)
        {
            uint32_t tmp = ((uint32_t)1 << (iter.required_field_index & 31));
            fields_seen.bitfield[iter.required_field_index >> 5] |= tmp;
        }

        if (!decode_field(stream, wire_type, &iter))
            return false;
    }

    /* Check that all elements of the last decoded fixed count field were present. */
    if (fixed_count_field != PB_SIZE_MAX &&
        fixed_count_size != fixed_count_total_size)
    {
        PB_RETURN_ERROR(stream, "wrong size for fixed count field");
    }

    /* Check that all required fields were present. */
    {
        pb_size_t req_field_count = iter.descriptor->required_field_count;

        if (req_field_count > 0)
        {
            pb_size_t i;

            if (req_field_count > PB_MAX_REQUIRED_FIELDS)
                req_field_count = PB_MAX_REQUIRED_FIELDS;

            /* Check the whole words */
            for (i = 0; i < (req_field_count >> 5); i++)
            {
                if (fields_seen.bitfield[i] != allbits)
                    PB_RETURN_ERROR(stream, "missing required field");
            }

            /* Check the remaining bits (if any) */
            if ((req_field_count & 31) != 0)
            {
                if (fields_seen.bitfield[req_field_count >> 5] !=
                    (allbits >> (uint_least8_t)(32 - (req_field_count & 31))))
                {
                    PB_RETURN_ERROR(stream, "missing required field");
                }
            }
        }
    }

    return true;
}
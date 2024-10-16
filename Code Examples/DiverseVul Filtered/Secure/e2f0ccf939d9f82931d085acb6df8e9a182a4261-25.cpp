static bool checkreturn decode_static_field(pb_istream_t *stream, pb_wire_type_t wire_type, pb_field_iter_t *field)
{
    switch (PB_HTYPE(field->type))
    {
        case PB_HTYPE_REQUIRED:
            return decode_basic_field(stream, wire_type, field);
            
        case PB_HTYPE_OPTIONAL:
            if (field->pSize != NULL)
                *(bool*)field->pSize = true;
            return decode_basic_field(stream, wire_type, field);
    
        case PB_HTYPE_REPEATED:
            if (wire_type == PB_WT_STRING
                && PB_LTYPE(field->type) <= PB_LTYPE_LAST_PACKABLE)
            {
                /* Packed array */
                bool status = true;
                pb_istream_t substream;
                pb_size_t *size = (pb_size_t*)field->pSize;
                field->pData = (char*)field->pField + field->data_size * (*size);

                if (!pb_make_string_substream(stream, &substream))
                    return false;

                while (substream.bytes_left > 0 && *size < field->array_size)
                {
                    if (!decode_basic_field(&substream, PB_WT_PACKED, field))
                    {
                        status = false;
                        break;
                    }
                    (*size)++;
                    field->pData = (char*)field->pData + field->data_size;
                }

                if (substream.bytes_left != 0)
                    PB_RETURN_ERROR(stream, "array overflow");
                if (!pb_close_string_substream(stream, &substream))
                    return false;

                return status;
            }
            else
            {
                /* Repeated field */
                pb_size_t *size = (pb_size_t*)field->pSize;
                field->pData = (char*)field->pField + field->data_size * (*size);

                if ((*size)++ >= field->array_size)
                    PB_RETURN_ERROR(stream, "array overflow");

                return decode_basic_field(stream, wire_type, field);
            }

        case PB_HTYPE_ONEOF:
            if (PB_LTYPE_IS_SUBMSG(field->type) &&
                *(pb_size_t*)field->pSize != field->tag)
            {
                /* We memset to zero so that any callbacks are set to NULL.
                 * This is because the callbacks might otherwise have values
                 * from some other union field.
                 * If callbacks are needed inside oneof field, use .proto
                 * option submsg_callback to have a separate callback function
                 * that can set the fields before submessage is decoded.
                 * pb_dec_submessage() will set any default values. */
                memset(field->pData, 0, (size_t)field->data_size);

                /* Set default values for the submessage fields. */
                if (field->submsg_desc->default_value != NULL ||
                    field->submsg_desc->field_callback != NULL ||
                    field->submsg_desc->submsg_info[0] != NULL)
                {
                    pb_field_iter_t submsg_iter;
                    if (pb_field_iter_begin(&submsg_iter, field->submsg_desc, field->pData))
                    {
                        if (!pb_message_set_to_defaults(&submsg_iter))
                            PB_RETURN_ERROR(stream, "failed to set defaults");
                    }
                }
            }
            *(pb_size_t*)field->pSize = field->tag;

            return decode_basic_field(stream, wire_type, field);

        default:
            PB_RETURN_ERROR(stream, "invalid field type");
    }
}
static bool checkreturn decode_basic_field(pb_istream_t *stream, pb_wire_type_t wire_type, pb_field_iter_t *field)
{
    switch (PB_LTYPE(field->type))
    {
        case PB_LTYPE_BOOL:
            if (wire_type != PB_WT_VARINT && wire_type != PB_WT_PACKED)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_dec_bool(stream, field);

        case PB_LTYPE_VARINT:
        case PB_LTYPE_UVARINT:
        case PB_LTYPE_SVARINT:
            if (wire_type != PB_WT_VARINT && wire_type != PB_WT_PACKED)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_dec_varint(stream, field);

        case PB_LTYPE_FIXED32:
            if (wire_type != PB_WT_32BIT && wire_type != PB_WT_PACKED)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_decode_fixed32(stream, field->pData);

        case PB_LTYPE_FIXED64:
            if (wire_type != PB_WT_64BIT && wire_type != PB_WT_PACKED)
                PB_RETURN_ERROR(stream, "wrong wire type");

#ifdef PB_CONVERT_DOUBLE_FLOAT
            if (field->data_size == sizeof(float))
            {
                return pb_decode_double_as_float(stream, (float*)field->pData);
            }
#endif

#ifdef PB_WITHOUT_64BIT
            PB_RETURN_ERROR(stream, "invalid data_size");
#else
            return pb_decode_fixed64(stream, field->pData);
#endif

        case PB_LTYPE_BYTES:
            if (wire_type != PB_WT_STRING)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_dec_bytes(stream, field);

        case PB_LTYPE_STRING:
            if (wire_type != PB_WT_STRING)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_dec_string(stream, field);

        case PB_LTYPE_SUBMESSAGE:
        case PB_LTYPE_SUBMSG_W_CB:
            if (wire_type != PB_WT_STRING)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_dec_submessage(stream, field);

        case PB_LTYPE_FIXED_LENGTH_BYTES:
            if (wire_type != PB_WT_STRING)
                PB_RETURN_ERROR(stream, "wrong wire type");

            return pb_dec_fixed_length_bytes(stream, field);

        default:
            PB_RETURN_ERROR(stream, "invalid field type");
    }
}
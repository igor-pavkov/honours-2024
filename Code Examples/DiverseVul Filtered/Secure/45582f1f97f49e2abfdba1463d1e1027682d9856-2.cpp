static bool checkreturn decode_basic_field(pb_istream_t *stream, pb_field_iter_t *field)
{
    switch (PB_LTYPE(field->type))
    {
        case PB_LTYPE_BOOL:
            return pb_dec_bool(stream, field);

        case PB_LTYPE_VARINT:
        case PB_LTYPE_UVARINT:
        case PB_LTYPE_SVARINT:
            return pb_dec_varint(stream, field);

        case PB_LTYPE_FIXED32:
        case PB_LTYPE_FIXED64:
            return pb_dec_fixed(stream, field);

        case PB_LTYPE_BYTES:
            return pb_dec_bytes(stream, field);

        case PB_LTYPE_STRING:
            return pb_dec_string(stream, field);

        case PB_LTYPE_SUBMESSAGE:
        case PB_LTYPE_SUBMSG_W_CB:
            return pb_dec_submessage(stream, field);

        case PB_LTYPE_FIXED_LENGTH_BYTES:
            return pb_dec_fixed_length_bytes(stream, field);

        default:
            PB_RETURN_ERROR(stream, "invalid field type");
    }
}
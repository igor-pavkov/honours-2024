static bool checkreturn check_wire_type(pb_wire_type_t wire_type, pb_field_iter_t *field)
{
    switch (PB_LTYPE(field->type))
    {
        case PB_LTYPE_BOOL:
        case PB_LTYPE_VARINT:
        case PB_LTYPE_UVARINT:
        case PB_LTYPE_SVARINT:
            return wire_type == PB_WT_VARINT;

        case PB_LTYPE_FIXED32:
            return wire_type == PB_WT_32BIT;

        case PB_LTYPE_FIXED64:
            return wire_type == PB_WT_64BIT;

        case PB_LTYPE_BYTES:
        case PB_LTYPE_STRING:
        case PB_LTYPE_SUBMESSAGE:
        case PB_LTYPE_SUBMSG_W_CB:
        case PB_LTYPE_FIXED_LENGTH_BYTES:
            return wire_type == PB_WT_STRING;

        default:
            return false;
    }
}
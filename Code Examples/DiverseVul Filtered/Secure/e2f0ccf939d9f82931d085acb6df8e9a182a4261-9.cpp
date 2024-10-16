static bool checkreturn pb_dec_varint(pb_istream_t *stream, const pb_field_iter_t *field)
{
    if (PB_LTYPE(field->type) == PB_LTYPE_UVARINT)
    {
        pb_uint64_t value, clamped;
        if (!pb_decode_varint(stream, &value))
            return false;

        /* Cast to the proper field size, while checking for overflows */
        if (field->data_size == sizeof(pb_uint64_t))
            clamped = *(pb_uint64_t*)field->pData = value;
        else if (field->data_size == sizeof(uint32_t))
            clamped = *(uint32_t*)field->pData = (uint32_t)value;
        else if (field->data_size == sizeof(uint_least16_t))
            clamped = *(uint_least16_t*)field->pData = (uint_least16_t)value;
        else if (field->data_size == sizeof(uint_least8_t))
            clamped = *(uint_least8_t*)field->pData = (uint_least8_t)value;
        else
            PB_RETURN_ERROR(stream, "invalid data_size");

        if (clamped != value)
            PB_RETURN_ERROR(stream, "integer too large");

        return true;
    }
    else
    {
        pb_uint64_t value;
        pb_int64_t svalue;
        pb_int64_t clamped;

        if (PB_LTYPE(field->type) == PB_LTYPE_SVARINT)
        {
            if (!pb_decode_svarint(stream, &svalue))
                return false;
        }
        else
        {
            if (!pb_decode_varint(stream, &value))
                return false;

            /* See issue 97: Google's C++ protobuf allows negative varint values to
            * be cast as int32_t, instead of the int64_t that should be used when
            * encoding. Nanopb versions before 0.2.5 had a bug in encoding. In order to
            * not break decoding of such messages, we cast <=32 bit fields to
            * int32_t first to get the sign correct.
            */
            if (field->data_size == sizeof(pb_int64_t))
                svalue = (pb_int64_t)value;
            else
                svalue = (int32_t)value;
        }

        /* Cast to the proper field size, while checking for overflows */
        if (field->data_size == sizeof(pb_int64_t))
            clamped = *(pb_int64_t*)field->pData = svalue;
        else if (field->data_size == sizeof(int32_t))
            clamped = *(int32_t*)field->pData = (int32_t)svalue;
        else if (field->data_size == sizeof(int_least16_t))
            clamped = *(int_least16_t*)field->pData = (int_least16_t)svalue;
        else if (field->data_size == sizeof(int_least8_t))
            clamped = *(int_least8_t*)field->pData = (int_least8_t)svalue;
        else
            PB_RETURN_ERROR(stream, "invalid data_size");

        if (clamped != svalue)
            PB_RETURN_ERROR(stream, "integer too large");

        return true;
    }
}
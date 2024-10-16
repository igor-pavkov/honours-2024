int ff_amf_match_string(const uint8_t *data, int size, const char *str)
{
    int len = strlen(str);
    int amf_len, type;

    if (size < 1)
        return 0;

    type = *data++;

    if (type != AMF_DATA_TYPE_LONG_STRING &&
        type != AMF_DATA_TYPE_STRING)
        return 0;

    if (type == AMF_DATA_TYPE_LONG_STRING) {
        if ((size -= 4 + 1) < 0)
            return 0;
        amf_len = bytestream_get_be32(&data);
    } else {
        if ((size -= 2 + 1) < 0)
            return 0;
        amf_len = bytestream_get_be16(&data);
    }

    if (amf_len > size)
        return 0;

    if (amf_len != len)
        return 0;

    return !memcmp(data, str, len);
}
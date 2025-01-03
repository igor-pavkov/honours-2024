int ff_amf_get_field_value(const uint8_t *data, const uint8_t *data_end,
                           const uint8_t *name, uint8_t *dst, int dst_size)
{
    GetByteContext gb;

    if (data >= data_end)
        return -1;

    bytestream2_init(&gb, data, data_end - data);

    return amf_get_field_value2(&gb, name, dst, dst_size);
}
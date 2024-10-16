void ff_amf_write_null(uint8_t **dst)
{
    bytestream_put_byte(dst, AMF_DATA_TYPE_NULL);
}
bool pb_decode_fixed64(pb_istream_t *stream, void *dest)
{
    #ifdef __BIG_ENDIAN__
    uint8_t *bytes = (uint8_t*)dest;
    uint8_t lebytes[8];
    
    if (!pb_read(stream, lebytes, 8))
        return false;
    
    bytes[0] = lebytes[7];
    bytes[1] = lebytes[6];
    bytes[2] = lebytes[5];
    bytes[3] = lebytes[4];
    bytes[4] = lebytes[3];
    bytes[5] = lebytes[2];
    bytes[6] = lebytes[1];
    bytes[7] = lebytes[0];
    return true;
    #else
    return pb_read(stream, (uint8_t*)dest, 8);
    #endif   
}
static apr_uint64_t ap_ntoh64(const apr_uint64_t *input)
{
    apr_uint64_t rval;
    unsigned char *data = (unsigned char *)&rval;
    if (APR_IS_BIGENDIAN) {
        return *input;
    }
    
    data[0] = *input >> 56;
    data[1] = *input >> 48;
    data[2] = *input >> 40;
    data[3] = *input >> 32;
    data[4] = *input >> 24;
    data[5] = *input >> 16;
    data[6] = *input >> 8;
    data[7] = *input >> 0;

    return rval;
}
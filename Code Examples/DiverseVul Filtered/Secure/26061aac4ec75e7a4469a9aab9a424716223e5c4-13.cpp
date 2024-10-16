static plist_t parse_real_node(const char **bnode, uint8_t size)
{
    plist_data_t data = plist_new_plist_data();
    float floatval = 0.0;
    uint8_t* buf;

    size = 1 << size;			// make length less misleading
    buf = malloc (size);
    memcpy (buf, *bnode, size);
    switch (size)
    {
    case sizeof(float):
        float_byte_convert(buf, size);
        floatval = *(float *) buf;
        data->realval = floatval;
        break;
    case sizeof(double):
        float_byte_convert(buf, size);
        data->realval = *(double *) buf;
        break;
    default:
        free(buf);
        free(data);
        return NULL;
    }
    free (buf);
    data->type = PLIST_REAL;
    data->length = sizeof(double);

    return node_create(NULL, data);
}
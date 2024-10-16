static plist_t parse_uint_node(const char **bnode, uint8_t size)
{
    plist_data_t data = plist_new_plist_data();

    size = 1 << size;			// make length less misleading
    switch (size)
    {
    case sizeof(uint8_t):
    case sizeof(uint16_t):
    case sizeof(uint32_t):
    case sizeof(uint64_t):
        memcpy(&data->intval, *bnode, size);
        data->intval = UINT_TO_HOST(&data->intval, size);
        data->length = sizeof(uint64_t);
        break;
    case 16:
        memcpy(&data->intval, (*bnode)+8, sizeof(uint64_t));
        data->intval = UINT_TO_HOST(&data->intval, sizeof(uint64_t));
        data->length = size;
        break;
    default:
        free(data);
        return NULL;
    };

    (*bnode) += size;
    data->type = PLIST_UINT;

    return node_create(NULL, data);
}
static plist_t parse_uid_node(const char **bnode, uint8_t size)
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
        break;
    default:
        free(data);
        return NULL;
    };

    data->type = PLIST_UID;
    data->length = sizeof(uint64_t);

    return node_create(NULL, data);
}
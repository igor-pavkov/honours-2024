static plist_t parse_unicode_node(const char **bnode, uint64_t size)
{
    plist_data_t data = plist_new_plist_data();
    uint64_t i = 0;
    uint16_t *unicodestr = NULL;
    char *tmpstr = NULL;
    long items_read = 0;
    long items_written = 0;

    data->type = PLIST_STRING;
    unicodestr = (uint16_t*) malloc(sizeof(uint16_t) * size);
    memcpy(unicodestr, *bnode, sizeof(uint16_t) * size);
    for (i = 0; i < size; i++)
        byte_convert((uint8_t *) (unicodestr + i), sizeof(uint16_t));

    tmpstr = plist_utf16_to_utf8(unicodestr, size, &items_read, &items_written);
    free(unicodestr);

    data->type = PLIST_STRING;
    data->strval = (char *) malloc(sizeof(char) * (items_written + 1));
    memcpy(data->strval, tmpstr, items_written);
    data->strval[items_written] = '\0';
    data->length = strlen(data->strval);
    free(tmpstr);
    return node_create(NULL, data);
}
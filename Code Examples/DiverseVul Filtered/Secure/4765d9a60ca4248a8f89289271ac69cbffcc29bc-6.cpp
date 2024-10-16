static plist_t parse_bin_node(struct bplist_data *bplist, const char** object)
{
    uint16_t type = 0;
    uint64_t size = 0;

    if (!object)
        return NULL;

    type = (**object) & BPLIST_MASK;
    size = (**object) & BPLIST_FILL;
    (*object)++;

    switch (type)
    {

    case BPLIST_NULL:
        switch (size)
        {

        case BPLIST_TRUE:
        {
            plist_data_t data = plist_new_plist_data();
            data->type = PLIST_BOOLEAN;
            data->boolval = TRUE;
            data->length = 1;
            return node_create(NULL, data);
        }

        case BPLIST_FALSE:
        {
            plist_data_t data = plist_new_plist_data();
            data->type = PLIST_BOOLEAN;
            data->boolval = FALSE;
            data->length = 1;
            return node_create(NULL, data);
        }

        case BPLIST_NULL:
        default:
            return NULL;
        }

    case BPLIST_UINT:
        if (*object - bplist->data + (uint64_t)(1 << size) >= bplist->size)
            return NULL;
        return parse_uint_node(object, size);

    case BPLIST_REAL:
        if (*object - bplist->data + (uint64_t)(1 << size) >= bplist->size)
            return NULL;
        return parse_real_node(object, size);

    case BPLIST_DATE:
        if (3 != size)
            return NULL;
        if (*object - bplist->data + (uint64_t)(1 << size) >= bplist->size)
            return NULL;
        return parse_date_node(object, size);

    case BPLIST_DATA:
        if (BPLIST_FILL == size) {
            uint8_t next_size = **object & BPLIST_FILL;
            if ((**object & BPLIST_MASK) != BPLIST_UINT)
                return NULL;
            (*object)++;
            size = UINT_TO_HOST(*object, (1 << next_size));
            (*object) += (1 << next_size);
        }

        if (*object - bplist->data + size >= bplist->size)
            return NULL;
        return parse_data_node(object, size);

    case BPLIST_STRING:
        if (BPLIST_FILL == size) {
            uint8_t next_size = **object & BPLIST_FILL;
            if ((**object & BPLIST_MASK) != BPLIST_UINT)
                return NULL;
            (*object)++;
            size = UINT_TO_HOST(*object, (1 << next_size));
            (*object) += (1 << next_size);
        }

        if (*object - bplist->data + size >= bplist->size)
            return NULL;
        return parse_string_node(object, size);

    case BPLIST_UNICODE:
        if (BPLIST_FILL == size) {
            uint8_t next_size = **object & BPLIST_FILL;
            if ((**object & BPLIST_MASK) != BPLIST_UINT)
                return NULL;
            (*object)++;
            size = UINT_TO_HOST(*object, (1 << next_size));
            (*object) += (1 << next_size);
        }

        if (*object - bplist->data + size * 2 >= bplist->size)
            return NULL;
        return parse_unicode_node(object, size);

    case BPLIST_SET:
    case BPLIST_ARRAY:
        if (BPLIST_FILL == size) {
            uint8_t next_size = **object & BPLIST_FILL;
            if ((**object & BPLIST_MASK) != BPLIST_UINT)
                return NULL;
            (*object)++;
            size = UINT_TO_HOST(*object, (1 << next_size));
            (*object) += (1 << next_size);
        }

        if (*object - bplist->data + size >= bplist->size)
            return NULL;
        return parse_array_node(bplist, object, size);

    case BPLIST_UID:
        return parse_uid_node(object, size);

    case BPLIST_DICT:
        if (BPLIST_FILL == size) {
	    uint8_t next_size = **object & BPLIST_FILL;
            if ((**object & BPLIST_MASK) != BPLIST_UINT)
                return NULL;
            (*object)++;
            size = UINT_TO_HOST(*object, (1 << next_size));
            (*object) += (1 << next_size);
        }

        if (*object - bplist->data + size >= bplist->size)
            return NULL;
        return parse_dict_node(bplist, object, size);
    default:
        return NULL;
    }
    return NULL;
}
static void write_dict(bytearray_t * bplist, node_t* node, hashtable_t* ref_table, uint8_t dict_param_size)
{
    uint64_t idx1 = 0;
    uint64_t idx2 = 0;
    uint8_t *buff = NULL;

    node_t* cur = NULL;
    uint64_t i = 0;

    uint64_t size = node_n_children(node) / 2;
    uint8_t marker = BPLIST_DICT | (size < 15 ? size : 0xf);
    byte_array_append(bplist, &marker, sizeof(uint8_t));
    if (size >= 15)
    {
        bytearray_t *int_buff = byte_array_new();
        write_int(int_buff, size);
        byte_array_append(bplist, int_buff->data, int_buff->len);
        byte_array_free(int_buff);
    }

    buff = (uint8_t *) malloc(size * 2 * dict_param_size);
    for (i = 0, cur = node_first_child(node); cur && i < size; cur = node_next_sibling(node_next_sibling(cur)), i++)
    {
        idx1 = *(uint64_t *) (hash_table_lookup(ref_table, cur));
#ifdef __BIG_ENDIAN__
	idx1 = idx1 << ((sizeof(uint64_t) - dict_param_size) * 8);
#endif
        memcpy(buff + i * dict_param_size, &idx1, dict_param_size);
        byte_convert(buff + i * dict_param_size, dict_param_size);

        idx2 = *(uint64_t *) (hash_table_lookup(ref_table, cur->next));
#ifdef __BIG_ENDIAN__
	idx2 = idx2 << ((sizeof(uint64_t) - dict_param_size) * 8);
#endif
        memcpy(buff + (i + size) * dict_param_size, &idx2, dict_param_size);
        byte_convert(buff + (i + size) * dict_param_size, dict_param_size);
    }

    //now append to bplist
    byte_array_append(bplist, buff, size * 2 * dict_param_size);
    free(buff);

}
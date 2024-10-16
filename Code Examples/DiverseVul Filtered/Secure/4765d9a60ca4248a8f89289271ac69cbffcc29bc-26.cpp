static void write_dict(bytearray_t * bplist, node_t* node, hashtable_t* ref_table, uint8_t ref_size)
{
    node_t* cur = NULL;
    uint64_t i = 0;

    uint64_t size = node_n_children(node) / 2;
    uint8_t marker = BPLIST_DICT | (size < 15 ? size : 0xf);
    byte_array_append(bplist, &marker, sizeof(uint8_t));
    if (size >= 15) {
        write_int(bplist, size);
    }

    for (i = 0, cur = node_first_child(node); cur && i < size; cur = node_next_sibling(node_next_sibling(cur)), i++) {
        uint64_t idx1 = *(uint64_t *) (hash_table_lookup(ref_table, cur));
        idx1 = be64toh(idx1);
        byte_array_append(bplist, (uint8_t*)&idx1 + (sizeof(uint64_t) - ref_size), ref_size);
    }

    for (i = 0, cur = node_first_child(node); cur && i < size; cur = node_next_sibling(node_next_sibling(cur)), i++) {
        uint64_t idx2 = *(uint64_t *) (hash_table_lookup(ref_table, cur->next));
        idx2 = be64toh(idx2);
        byte_array_append(bplist, (uint8_t*)&idx2 + (sizeof(uint64_t) - ref_size), ref_size);
    }
}
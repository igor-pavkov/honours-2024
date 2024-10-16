static plist_t parse_bin_node_at_index(struct bplist_data *bplist, uint32_t node_index)
{
    int i = 0;
    const char* ptr = NULL;
    plist_t plist = NULL;
    const char* idx_ptr = NULL;

    if (node_index > bplist->num_objects)
        return NULL;

    idx_ptr = bplist->offset_table + node_index * bplist->offset_size;
    if (idx_ptr < bplist->offset_table ||
        idx_ptr >= bplist->offset_table + bplist->num_objects * bplist->offset_size)
        return NULL;

    ptr = bplist->data + UINT_TO_HOST(idx_ptr, bplist->offset_size);
    /* make sure the node offset is in a sane range */
    if ((ptr < bplist->data) || (ptr >= bplist->offset_table)) {
        return NULL;
    }

    /* store node_index for current recursion level */
    bplist->used_indexes[bplist->level] = node_index;
    /* recursion check */
    if (bplist->level > 0) {
        for (i = bplist->level-1; i >= 0; i--) {
            if (bplist->used_indexes[i] == bplist->used_indexes[bplist->level]) {
                fprintf(stderr, "Recursion detected in binary plist. Aborting.\n");
                return NULL;
            }
        }
    }

    /* finally parse node */
    bplist->level++;
    plist = parse_bin_node(bplist, &ptr);
    bplist->level--;
    return plist;
}
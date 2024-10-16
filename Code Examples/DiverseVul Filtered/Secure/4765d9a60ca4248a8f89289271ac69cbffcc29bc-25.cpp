PLIST_API void plist_from_bin(const char *plist_bin, uint32_t length, plist_t * plist)
{
    bplist_trailer_t *trailer = NULL;
    uint8_t offset_size = 0;
    uint8_t ref_size = 0;
    uint64_t num_objects = 0;
    uint64_t root_object = 0;
    char *offset_table = NULL;

    //first check we have enough data
    if (!(length >= BPLIST_MAGIC_SIZE + BPLIST_VERSION_SIZE + sizeof(bplist_trailer_t)))
        return;
    //check that plist_bin in actually a plist
    if (memcmp(plist_bin, BPLIST_MAGIC, BPLIST_MAGIC_SIZE) != 0)
        return;
    //check for known version
    if (memcmp(plist_bin + BPLIST_MAGIC_SIZE, BPLIST_VERSION, BPLIST_VERSION_SIZE) != 0)
        return;

    //now parse trailer
    trailer = (bplist_trailer_t*)(plist_bin + (length - sizeof(bplist_trailer_t)));

    offset_size = trailer->offset_size;
    ref_size = trailer->ref_size;
    num_objects = be64toh(trailer->num_objects);
    root_object = be64toh(trailer->root_object_index);
    offset_table = (char *)(plist_bin + be64toh(trailer->offset_table_offset));

    if (num_objects == 0)
        return;

    if (offset_size == 0)
        return;

    if (ref_size == 0)
        return;

    if (root_object >= num_objects)
        return;

    if (offset_table < plist_bin || offset_table >= plist_bin + length)
        return;

    if (offset_table + num_objects * offset_size >= plist_bin + length)
        return;

    if (sizeof(uint32_t) * num_objects < num_objects)
        return;

    struct bplist_data bplist;
    bplist.data = plist_bin;
    bplist.size = length;
    bplist.num_objects = num_objects;
    bplist.ref_size = ref_size;
    bplist.offset_size = offset_size;
    bplist.offset_table = offset_table;
    bplist.level = 0;
    bplist.used_indexes = (uint32_t*)malloc(sizeof(uint32_t) * num_objects);

    if (!bplist.used_indexes)
        return;

    *plist = parse_bin_node_at_index(&bplist, root_object);

    free(bplist.used_indexes);
}
typedef struct __attribute__((packed)) {
    uint8_t unused[6];
    uint8_t offset_size;
    uint8_t ref_size;
    uint64_t num_objects;
    uint64_t root_object_index;
    uint64_t offset_table_offset;
} bplist_trailer_t;
JanetArray *janet_array(int32_t capacity) {
    JanetArray *array = janet_gcalloc(JANET_MEMORY_ARRAY, sizeof(JanetArray));
    Janet *data = NULL;
    if (capacity > 0) {
        janet_vm.next_collection += capacity * sizeof(Janet);
        data = (Janet *) janet_malloc(sizeof(Janet) * (size_t) capacity);
        if (NULL == data) {
            JANET_OUT_OF_MEMORY;
        }
    }
    array->count = 0;
    array->capacity = capacity;
    array->data = data;
    return array;
}
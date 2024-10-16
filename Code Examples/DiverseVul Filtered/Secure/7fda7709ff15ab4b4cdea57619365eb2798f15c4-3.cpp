JanetArray *janet_array_n(const Janet *elements, int32_t n) {
    JanetArray *array = janet_gcalloc(JANET_MEMORY_ARRAY, sizeof(JanetArray));
    array->capacity = n;
    array->count = n;
    array->data = janet_malloc(sizeof(Janet) * (size_t) n);
    if (!array->data) {
        JANET_OUT_OF_MEMORY;
    }
    safe_memcpy(array->data, elements, sizeof(Janet) * n);
    return array;
}
void janet_array_ensure(JanetArray *array, int32_t capacity, int32_t growth) {
    Janet *newData;
    Janet *old = array->data;
    if (capacity <= array->capacity) return;
    int64_t new_capacity = ((int64_t) capacity) * growth;
    if (new_capacity > INT32_MAX) new_capacity = INT32_MAX;
    capacity = (int32_t) new_capacity;
    newData = janet_realloc(old, capacity * sizeof(Janet));
    if (NULL == newData) {
        JANET_OUT_OF_MEMORY;
    }
    janet_vm.next_collection += (capacity - array->capacity) * sizeof(Janet);
    array->data = newData;
    array->capacity = capacity;
}
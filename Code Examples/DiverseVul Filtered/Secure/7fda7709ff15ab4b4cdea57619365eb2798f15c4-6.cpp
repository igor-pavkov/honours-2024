void janet_array_push(JanetArray *array, Janet x) {
    if (array->count == INT32_MAX) {
        janet_panic("array overflow");
    }
    int32_t newcount = array->count + 1;
    janet_array_ensure(array, newcount, 2);
    array->data[array->count] = x;
    array->count = newcount;
}
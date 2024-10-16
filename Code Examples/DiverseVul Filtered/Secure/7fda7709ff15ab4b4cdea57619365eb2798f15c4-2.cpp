void janet_array_setcount(JanetArray *array, int32_t count) {
    if (count < 0)
        return;
    if (count > array->count) {
        int32_t i;
        janet_array_ensure(array, count, 1);
        for (i = array->count; i < count; i++) {
            array->data[i] = janet_wrap_nil();
        }
    }
    array->count = count;
}
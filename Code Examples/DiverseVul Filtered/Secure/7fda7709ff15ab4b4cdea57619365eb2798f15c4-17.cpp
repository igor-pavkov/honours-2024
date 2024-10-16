Janet janet_array_pop(JanetArray *array) {
    if (array->count) {
        return array->data[--array->count];
    } else {
        return janet_wrap_nil();
    }
}
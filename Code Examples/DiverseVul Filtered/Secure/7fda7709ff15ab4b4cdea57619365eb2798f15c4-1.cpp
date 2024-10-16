Janet janet_array_peek(JanetArray *array) {
    if (array->count) {
        return array->data[array->count - 1];
    } else {
        return janet_wrap_nil();
    }
}
JANET_CORE_FN(cfun_array_trim,
              "(array/trim arr)",
              "Set the backing capacity of an array to its current length. Returns the modified array.") {
    janet_fixarity(argc, 1);
    JanetArray *array = janet_getarray(argv, 0);
    if (array->count) {
        if (array->count < array->capacity) {
            Janet *newData = janet_realloc(array->data, array->count * sizeof(Janet));
            if (NULL == newData) {
                JANET_OUT_OF_MEMORY;
            }
            array->data = newData;
            array->capacity = array->count;
        }
    } else {
        array->capacity = 0;
        janet_free(array->data);
        array->data = NULL;
    }
    return argv[0];
}
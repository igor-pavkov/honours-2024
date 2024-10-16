JANET_CORE_FN(cfun_array_remove,
              "(array/remove arr at &opt n)",
              "Remove up to `n` elements starting at index `at` in array `arr`. `at` can index from "
              "the end of the array with a negative index, and `n` must be a non-negative integer. "
              "By default, `n` is 1. "
              "Returns the array.") {
    janet_arity(argc, 2, 3);
    JanetArray *array = janet_getarray(argv, 0);
    int32_t at = janet_getinteger(argv, 1);
    int32_t n = 1;
    if (at < 0) {
        at = array->count + at + 1;
    }
    if (at < 0 || at > array->count)
        janet_panicf("removal index %d out of range [0,%d]", at, array->count);
    if (argc == 3) {
        n = janet_getinteger(argv, 2);
        if (n < 0)
            janet_panicf("expected non-negative integer for argument n, got %v", argv[2]);
    }
    if (at + n > array->count) {
        n = array->count - at;
    }
    memmove(array->data + at,
            array->data + at + n,
            (array->count - at - n) * sizeof(Janet));
    array->count -= n;
    return argv[0];
}
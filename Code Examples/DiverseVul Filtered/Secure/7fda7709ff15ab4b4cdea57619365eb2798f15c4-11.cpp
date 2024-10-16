JANET_CORE_FN(cfun_array_insert,
              "(array/insert arr at & xs)",
              "Insert all `xs` into array `arr` at index `at`. `at` should be an integer between "
              "0 and the length of the array. A negative value for `at` will index backwards from "
              "the end of the array, such that inserting at -1 appends to the array. "
              "Returns the array.") {
    size_t chunksize, restsize;
    janet_arity(argc, 2, -1);
    JanetArray *array = janet_getarray(argv, 0);
    int32_t at = janet_getinteger(argv, 1);
    if (at < 0) {
        at = array->count + at + 1;
    }
    if (at < 0 || at > array->count)
        janet_panicf("insertion index %d out of range [0,%d]", at, array->count);
    chunksize = (argc - 2) * sizeof(Janet);
    restsize = (array->count - at) * sizeof(Janet);
    if (INT32_MAX - (argc - 2) < array->count) {
        janet_panic("array overflow");
    }
    janet_array_ensure(array, array->count + argc - 2, 2);
    if (restsize) {
        memmove(array->data + at + argc - 2,
                array->data + at,
                restsize);
    }
    safe_memcpy(array->data + at, argv + 2, chunksize);
    array->count += (argc - 2);
    return argv[0];
}
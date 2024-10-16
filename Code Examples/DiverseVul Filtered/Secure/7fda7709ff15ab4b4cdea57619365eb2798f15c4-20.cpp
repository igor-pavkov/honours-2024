JANET_CORE_FN(cfun_array_push,
              "(array/push arr x)",
              "Insert an element in the end of an array. Modifies the input array and returns it.") {
    janet_arity(argc, 1, -1);
    JanetArray *array = janet_getarray(argv, 0);
    if (INT32_MAX - argc + 1 <= array->count) {
        janet_panic("array overflow");
    }
    int32_t newcount = array->count - 1 + argc;
    janet_array_ensure(array, newcount, 2);
    if (argc > 1) memcpy(array->data + array->count, argv + 1, (size_t)(argc - 1) * sizeof(Janet));
    array->count = newcount;
    return argv[0];
}
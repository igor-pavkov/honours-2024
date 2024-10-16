JANET_CORE_FN(cfun_array_fill,
              "(array/fill arr &opt value)",
              "Replace all elements of an array with `value` (defaulting to nil) without changing the length of the array. "
              "Returns the modified array.") {
    janet_arity(argc, 1, 2);
    JanetArray *array = janet_getarray(argv, 0);
    Janet x = (argc == 2) ? argv[1] : janet_wrap_nil();
    for (int32_t i = 0; i < array->count; i++) {
        array->data[i] = x;
    }
    return argv[0];
}
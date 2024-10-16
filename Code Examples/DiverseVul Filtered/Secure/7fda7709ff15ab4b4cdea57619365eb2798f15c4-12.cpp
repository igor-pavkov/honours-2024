JANET_CORE_FN(cfun_array_ensure,
              "(array/ensure arr capacity growth)",
              "Ensures that the memory backing the array is large enough for `capacity` "
              "items at the given rate of growth. `capacity` and `growth` must be integers. "
              "If the backing capacity is already enough, then this function does nothing. "
              "Otherwise, the backing memory will be reallocated so that there is enough space.") {
    janet_fixarity(argc, 3);
    JanetArray *array = janet_getarray(argv, 0);
    int32_t newcount = janet_getinteger(argv, 1);
    int32_t growth = janet_getinteger(argv, 2);
    if (newcount < 1) janet_panic("expected positive integer");
    janet_array_ensure(array, newcount, growth);
    return argv[0];
}
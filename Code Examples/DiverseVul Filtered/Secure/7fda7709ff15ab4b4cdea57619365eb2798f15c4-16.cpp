JANET_CORE_FN(cfun_array_new,
              "(array/new capacity)",
              "Creates a new empty array with a pre-allocated capacity. The same as "
              "`(array)` but can be more efficient if the maximum size of an array is known.") {
    janet_fixarity(argc, 1);
    int32_t cap = janet_getinteger(argv, 0);
    JanetArray *array = janet_array(cap);
    return janet_wrap_array(array);
}
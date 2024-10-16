JANET_CORE_FN(cfun_array_clear,
              "(array/clear arr)",
              "Empties an array, setting it's count to 0 but does not free the backing capacity. "
              "Returns the modified array.") {
    janet_fixarity(argc, 1);
    JanetArray *array = janet_getarray(argv, 0);
    array->count = 0;
    return argv[0];
}
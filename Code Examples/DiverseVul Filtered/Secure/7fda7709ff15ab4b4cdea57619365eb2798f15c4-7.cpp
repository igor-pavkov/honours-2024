JANET_CORE_FN(cfun_array_peek,
              "(array/peek arr)",
              "Returns the last element of the array. Does not modify the array.") {
    janet_fixarity(argc, 1);
    JanetArray *array = janet_getarray(argv, 0);
    return janet_array_peek(array);
}
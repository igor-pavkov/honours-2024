JANET_CORE_FN(cfun_array_pop,
              "(array/pop arr)",
              "Remove the last element of the array and return it. If the array is empty, will return nil. Modifies "
              "the input array.") {
    janet_fixarity(argc, 1);
    JanetArray *array = janet_getarray(argv, 0);
    return janet_array_pop(array);
}
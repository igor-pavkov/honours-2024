JANET_CORE_FN(cfun_array_concat,
              "(array/concat arr & parts)",
              "Concatenates a variable number of arrays (and tuples) into the first argument, "
              "which must be an array. If any of the parts are arrays or tuples, their elements will "
              "be inserted into the array. Otherwise, each part in `parts` will be appended to `arr` in order. "
              "Return the modified array `arr`.") {
    int32_t i;
    janet_arity(argc, 1, -1);
    JanetArray *array = janet_getarray(argv, 0);
    for (i = 1; i < argc; i++) {
        switch (janet_type(argv[i])) {
            default:
                janet_array_push(array, argv[i]);
                break;
            case JANET_ARRAY:
            case JANET_TUPLE: {
                int32_t j, len = 0;
                const Janet *vals = NULL;
                janet_indexed_view(argv[i], &vals, &len);
                if (array->data == vals) {
                    int32_t newcount = array->count + len;
                    janet_array_ensure(array, newcount, 2);
                    janet_indexed_view(argv[i], &vals, &len);
                }
                for (j = 0; j < len; j++)
                    janet_array_push(array, vals[j]);
            }
            break;
        }
    }
    return janet_wrap_array(array);
}
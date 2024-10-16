JANET_CORE_FN(cfun_array_slice,
              "(array/slice arrtup &opt start end)",
              "Takes a slice of array or tuple from `start` to `end`. The range is half open, "
              "[start, end). Indexes can also be negative, indicating indexing from the "
              "end of the array. By default, `start` is 0 and `end` is the length of the array. "
              "Note that index -1 is synonymous with index `(length arrtup)` to allow a full "
              "negative slice range. Returns a new array.") {
    JanetView view = janet_getindexed(argv, 0);
    JanetRange range = janet_getslice(argc, argv);
    JanetArray *array = janet_array(range.end - range.start);
    if (array->data)
        memcpy(array->data, view.items + range.start, sizeof(Janet) * (range.end - range.start));
    array->count = range.end - range.start;
    return janet_wrap_array(array);
}
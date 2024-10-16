static int parse_size(const char *str) {
    static const char *size_names[] =
        { "byte", "dword", "oword", "qword", "tword", "word", "yword" };
    static const int sizes[] =
        { 0, 1, 4, 16, 8, 10, 2, 32 };

    return sizes[bsii(str, size_names, ARRAY_SIZE(size_names))+1];
}
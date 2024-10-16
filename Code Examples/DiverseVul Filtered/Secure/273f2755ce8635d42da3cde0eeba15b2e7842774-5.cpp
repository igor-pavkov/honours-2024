static int scanbraces(const char* in) {
    if (strncmp(in, "{\\an", 4) != 0) {
        return 0;
    }
    if (!isdigit(in[4])) {
        return 0;
    }
    if (in[5] != '}') {
        return 0;
    }
    return 1;
}
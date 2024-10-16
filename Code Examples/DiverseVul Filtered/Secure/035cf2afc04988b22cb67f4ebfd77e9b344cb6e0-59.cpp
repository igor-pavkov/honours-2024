int linenoiseHistoryLoad(const char* filename) {
    FILE* fp = fopen(filename, "rt");
    if (fp == NULL) {
        return -1;
    }

    char buf[LINENOISE_MAX_LINE];
    while (fgets(buf, LINENOISE_MAX_LINE, fp) != NULL) {
        char* p = strchr(buf, '\r');
        if (!p) {
            p = strchr(buf, '\n');
        }
        if (p) {
            *p = '\0';
        }
        if (p != buf) {
            linenoiseHistoryAdd(buf);
        }
    }
    fclose(fp);
    return 0;
}
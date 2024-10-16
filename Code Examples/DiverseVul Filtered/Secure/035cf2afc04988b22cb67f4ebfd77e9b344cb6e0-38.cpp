int linenoiseHistoryAdd(const char* line) {
    if (historyMaxLen == 0) {
        return 0;
    }
    if (history == NULL) {
        history = reinterpret_cast<UChar8**>(malloc(sizeof(UChar8*) * historyMaxLen));
        if (history == NULL) {
            return 0;
        }
        memset(history, 0, (sizeof(char*) * historyMaxLen));
    }
    UChar8* linecopy = reinterpret_cast<UChar8*>(strdup(line));
    if (!linecopy) {
        return 0;
    }
    if (historyLen == historyMaxLen) {
        free(history[0]);
        memmove(history, history + 1, sizeof(char*) * (historyMaxLen - 1));
        --historyLen;
        if (--historyPreviousIndex < -1) {
            historyPreviousIndex = -2;
        }
    }

    // convert newlines in multi-line code to spaces before storing
    UChar8* p = linecopy;
    while (*p) {
        if (*p == '\n') {
            *p = ' ';
        }
        ++p;
    }
    history[historyLen] = linecopy;
    ++historyLen;
    return 1;
}
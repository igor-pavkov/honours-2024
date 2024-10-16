int linenoiseHistorySetMaxLen(int len) {
    if (len < 1) {
        return 0;
    }
    if (history) {
        int tocopy = historyLen;
        UChar8** newHistory = reinterpret_cast<UChar8**>(malloc(sizeof(UChar8*) * len));
        if (newHistory == NULL) {
            return 0;
        }
        if (len < tocopy) {
            tocopy = len;
        }
        memcpy(newHistory, history + historyMaxLen - tocopy, sizeof(UChar8*) * tocopy);
        free(history);
        history = newHistory;
    }
    historyMaxLen = len;
    if (historyLen > historyMaxLen) {
        historyLen = historyMaxLen;
    }
    return 1;
}
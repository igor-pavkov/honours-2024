void linenoiseHistoryFree(void) {
    if (history) {
        for (int j = 0; j < historyLen; ++j)
            free(history[j]);
        historyLen = 0;
        free(history);
        history = 0;
    }
}
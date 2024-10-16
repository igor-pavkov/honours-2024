char* linenoise(const char* prompt) {
    if (isatty(STDIN_FILENO)) {  // input is from a terminal
        UChar32 buf32[LINENOISE_MAX_LINE];
        char charWidths[LINENOISE_MAX_LINE];
        if (!preloadErrorMessage.empty()) {
            printf("%s", preloadErrorMessage.c_str());
            fflush(stdout);
            preloadErrorMessage.clear();
        }
        PromptInfo pi(reinterpret_cast<const UChar8*>(prompt), getScreenColumns());
        if (isUnsupportedTerm()) {
            if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
                return 0;
            fflush(stdout);
            if (preloadedBufferContents.empty()) {
                unique_ptr<char[]> buf8(new char[LINENOISE_MAX_LINE]);
                if (fgets(buf8.get(), LINENOISE_MAX_LINE, stdin) == NULL) {
                    return NULL;
                }
                size_t len = strlen(buf8.get());
                while (len && (buf8[len - 1] == '\n' || buf8[len - 1] == '\r')) {
                    --len;
                    buf8[len] = '\0';
                }
                return strdup(buf8.get());  // caller must free buffer
            } else {
                char* buf8 = strdup(preloadedBufferContents.c_str());
                preloadedBufferContents.clear();
                return buf8;  // caller must free buffer
            }
        } else {
            if (enableRawMode() == -1) {
                return NULL;
            }
            InputBuffer ib(buf32, charWidths, LINENOISE_MAX_LINE);
            if (!preloadedBufferContents.empty()) {
                ib.preloadBuffer(reinterpret_cast<const UChar8*>(preloadedBufferContents.c_str()));
                preloadedBufferContents.clear();
            }
            int count = ib.getInputLine(pi);
            disableRawMode();
            printf("\n");
            if (count == -1) {
                return NULL;
            }
            size_t bufferSize = sizeof(UChar32) * ib.length() + 1;
            unique_ptr<UChar8[]> buf8(new UChar8[bufferSize]);
            copyString32to8(buf8.get(), buf32, bufferSize);
            return strdup(reinterpret_cast<char*>(buf8.get()));  // caller must free buffer
        }
    } else {  // input not from a terminal, we should work with piped input, i.e. redirected stdin
        unique_ptr<char[]> buf8(new char[LINENOISE_MAX_LINE]);
        if (fgets(buf8.get(), LINENOISE_MAX_LINE, stdin) == NULL) {
            return NULL;
        }

        // if fgets() gave us the newline, remove it
        int count = strlen(buf8.get());
        if (count > 0 && buf8[count - 1] == '\n') {
            --count;
            buf8[count] = '\0';
        }
        return strdup(buf8.get());  // caller must free buffer
    }
}
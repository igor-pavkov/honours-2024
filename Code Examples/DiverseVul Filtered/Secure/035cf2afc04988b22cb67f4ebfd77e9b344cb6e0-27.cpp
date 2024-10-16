    void preloadBuffer(const UChar8* preloadText) {
        size_t ucharCount;
        int errorCode;
        copyString8to32(buf32, preloadText, buflen + 1, ucharCount, errorCode);
        recomputeCharacterWidths(buf32, charWidths, ucharCount);
        len = ucharCount;
        pos = ucharCount;
    }
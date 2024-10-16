static UChar32 escLeftBracket3Routine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    return doDispatch(c, escLeftBracket3Dispatch);
}
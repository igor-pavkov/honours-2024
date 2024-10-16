static UChar32 escLeftBracket8Routine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    return doDispatch(c, escLeftBracket8Dispatch);
}
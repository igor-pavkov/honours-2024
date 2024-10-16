static UChar32 escLeftBracketRoutine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    return doDispatch(c, escLeftBracketDispatch);
}
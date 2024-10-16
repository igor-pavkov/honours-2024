static UChar32 escLeftBracket1SemicolonRoutine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    return doDispatch(c, escLeftBracket1SemicolonDispatch);
}
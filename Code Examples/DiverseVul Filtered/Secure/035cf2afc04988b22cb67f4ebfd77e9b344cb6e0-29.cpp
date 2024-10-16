static UChar32 escLeftBracket1Semicolon3Routine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    thisKeyMetaCtrl |= META;
    return doDispatch(c, escLeftBracket1Semicolon3or5Dispatch);
}
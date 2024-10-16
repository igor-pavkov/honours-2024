static UChar32 escLeftBracket1Semicolon5Routine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    thisKeyMetaCtrl |= CTRL;
    return doDispatch(c, escLeftBracket1Semicolon3or5Dispatch);
}
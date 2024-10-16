static UChar32 escORoutine(UChar32 c) {
    c = readUnicodeCharacter();
    if (c == 0)
        return 0;
    return doDispatch(c, escODispatch);
}
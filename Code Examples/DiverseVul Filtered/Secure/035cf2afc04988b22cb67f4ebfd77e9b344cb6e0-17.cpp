static UChar32 doDispatch(UChar32 c, CharacterDispatch& dispatchTable) {
    for (unsigned int i = 0; i < dispatchTable.len; ++i) {
        if (static_cast<unsigned char>(dispatchTable.chars[i]) == c) {
            return dispatchTable.dispatch[i](c);
        }
    }
    return dispatchTable.dispatch[dispatchTable.len](c);
}
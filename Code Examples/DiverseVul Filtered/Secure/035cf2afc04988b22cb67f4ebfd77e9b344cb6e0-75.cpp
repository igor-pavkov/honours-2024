    void updateSearchText(const UChar32* textPtr) {
        Utf32String tempUnicode(textPtr);
        searchTextLen = tempUnicode.chars();
        searchText = tempUnicode;
        updateSearchPrompt();
    }
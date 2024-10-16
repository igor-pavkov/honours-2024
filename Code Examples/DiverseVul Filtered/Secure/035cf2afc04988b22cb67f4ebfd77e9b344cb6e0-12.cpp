    void updateSearchPrompt(void) {
        const Utf32String* basePrompt =
            (direction > 0) ? &forwardSearchBasePrompt : &reverseSearchBasePrompt;
        size_t promptStartLength = basePrompt->length();
        promptChars = promptStartLength + searchTextLen + endSearchBasePrompt.length();
        Utf32String tempUnicode(promptChars + 1);
        memcpy(tempUnicode.get(), basePrompt->get(), sizeof(UChar32) * promptStartLength);
        memcpy(&tempUnicode[promptStartLength], searchText.get(), sizeof(UChar32) * searchTextLen);
        size_t endIndex = promptStartLength + searchTextLen;
        memcpy(&tempUnicode[endIndex],
               endSearchBasePrompt.get(),
               sizeof(UChar32) * (endSearchBasePrompt.length() + 1));
        tempUnicode.initFromBuffer();
        promptText = tempUnicode;
    }
    DynamicPrompt(PromptBase& pi, int initialDirection)
        : searchTextLen(0), direction(initialDirection) {
        promptScreenColumns = pi.promptScreenColumns;
        promptCursorRowOffset = 0;
        Utf32String emptyString(1);
        searchText = emptyString;
        const Utf32String* basePrompt =
            (direction > 0) ? &forwardSearchBasePrompt : &reverseSearchBasePrompt;
        size_t promptStartLength = basePrompt->length();
        promptChars = promptStartLength + endSearchBasePrompt.length();
        promptLastLinePosition =
            promptChars;  // TODO fix this, we are asssuming that the history prompt won't wrap (!)
        promptPreviousLen = promptChars;
        Utf32String tempUnicode(promptChars + 1);
        memcpy(tempUnicode.get(), basePrompt->get(), sizeof(UChar32) * promptStartLength);
        memcpy(&tempUnicode[promptStartLength],
               endSearchBasePrompt.get(),
               sizeof(UChar32) * (endSearchBasePrompt.length() + 1));
        tempUnicode.initFromBuffer();
        promptText = tempUnicode;
        calculateScreenPosition(
            0, 0, pi.promptScreenColumns, promptChars, promptIndentation, promptExtraLines);
    }
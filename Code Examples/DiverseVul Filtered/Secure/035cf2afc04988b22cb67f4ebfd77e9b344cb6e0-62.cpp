void linenoiseAddCompletion(linenoiseCompletions* lc, const char* str) {
    lc->completionStrings.push_back(Utf32String(reinterpret_cast<const UChar8*>(str)));
}
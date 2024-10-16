static void recomputeCharacterWidths(const UChar32* text, char* widths, int charCount) {
    for (int i = 0; i < charCount; ++i) {
        widths[i] = mk_wcwidth(text[i]);
    }
}
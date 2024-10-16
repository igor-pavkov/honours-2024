static UChar32 readUnicodeCharacter(void) {
    static UChar8 utf8String[5];
    static size_t utf8Count = 0;
    while (true) {
        UChar8 c;
        if (read(0, &c, 1) <= 0)
            return 0;
        if (c <= 0x7F) {  // short circuit ASCII
            utf8Count = 0;
            return c;
        } else if (utf8Count < sizeof(utf8String) - 1) {
            utf8String[utf8Count++] = c;
            utf8String[utf8Count] = 0;
            UChar32 unicodeChar[2];
            size_t ucharCount;
            int errorCode;
            copyString8to32(unicodeChar, utf8String, 2, ucharCount, errorCode);
            if (ucharCount && errorCode == 0) {
                utf8Count = 0;
                return unicodeChar[0];
            }
        } else {
            utf8Count = 0;  // this shouldn't happen: got four bytes but no UTF-8 character
        }
    }
}
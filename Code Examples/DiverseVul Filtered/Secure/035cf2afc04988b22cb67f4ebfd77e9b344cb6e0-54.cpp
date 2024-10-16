static bool isControlChar(UChar32 testChar) {
    return (testChar < ' ') ||                   // C0 controls
        (testChar >= 0x7F && testChar <= 0x9F);  // DEL and C1 controls
}
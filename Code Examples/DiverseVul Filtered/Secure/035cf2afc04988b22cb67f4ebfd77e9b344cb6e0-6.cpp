static void calculateScreenPosition(
    int x, int y, int screenColumns, int charCount, int& xOut, int& yOut) {
    xOut = x;
    yOut = y;
    int charsRemaining = charCount;
    while (charsRemaining > 0) {
        int charsThisRow =
            (x + charsRemaining < screenColumns) ? charsRemaining : screenColumns - x;
        xOut = x + charsThisRow;
        yOut = y;
        charsRemaining -= charsThisRow;
        x = 0;
        ++y;
    }
    if (xOut == screenColumns) {  // we have to special-case line wrap
        xOut = 0;
        ++yOut;
    }
}
int InputBuffer::completeLine(PromptBase& pi) {
    linenoiseCompletions lc;
    char c = 0;

    // completionCallback() expects a parsable entity, so find the previous break character and
    // extract a copy to parse.  we also handle the case where tab is hit while not at end-of-line.
    int startIndex = pos;
    while (--startIndex >= 0) {
        if (strchr(breakChars, buf32[startIndex])) {
            break;
        }
    }
    ++startIndex;
    int itemLength = pos - startIndex;
    Utf32String unicodeCopy(&buf32[startIndex], itemLength);
    Utf8String parseItem(unicodeCopy);

    // get a list of completions
    completionCallback(reinterpret_cast<char*>(parseItem.get()), &lc);

    // if no completions, we are done
    if (lc.completionStrings.size() == 0) {
        beep();
        freeCompletions(&lc);
        return 0;
    }

    // at least one completion
    int longestCommonPrefix = 0;
    int displayLength = 0;
    if (lc.completionStrings.size() == 1) {
        longestCommonPrefix = lc.completionStrings[0].length();
    } else {
        bool keepGoing = true;
        while (keepGoing) {
            for (size_t j = 0; j < lc.completionStrings.size() - 1; ++j) {
                char c1 = lc.completionStrings[j][longestCommonPrefix];
                char c2 = lc.completionStrings[j + 1][longestCommonPrefix];
                if ((0 == c1) || (0 == c2) || (c1 != c2)) {
                    keepGoing = false;
                    break;
                }
            }
            if (keepGoing) {
                ++longestCommonPrefix;
            }
        }
    }
    if (lc.completionStrings.size() != 1) {  // beep if ambiguous
        beep();
    }

    // if we can extend the item, extend it and return to main loop
    if (longestCommonPrefix > itemLength) {
        displayLength = len + longestCommonPrefix - itemLength;
        if (displayLength > buflen) {
            longestCommonPrefix -= displayLength - buflen;  // don't overflow buffer
            displayLength = buflen;                         // truncate the insertion
            beep();                                         // and make a noise
        }
        Utf32String displayText(displayLength + 1);
        memcpy(displayText.get(), buf32, sizeof(UChar32) * startIndex);
        memcpy(&displayText[startIndex],
               &lc.completionStrings[0][0],
               sizeof(UChar32) * longestCommonPrefix);
        int tailIndex = startIndex + longestCommonPrefix;
        memcpy(&displayText[tailIndex],
               &buf32[pos],
               sizeof(UChar32) * (displayLength - tailIndex + 1));
        copyString32(buf32, displayText.get(), buflen + 1);
        pos = startIndex + longestCommonPrefix;
        len = displayLength;
        refreshLine(pi);
        return 0;
    }

    // we can't complete any further, wait for second tab
    do {
        c = linenoiseReadChar();
        c = cleanupCtrl(c);
    } while (c == static_cast<char>(-1));

    // if any character other than tab, pass it to the main loop
    if (c != ctrlChar('I')) {
        freeCompletions(&lc);
        return c;
    }

    // we got a second tab, maybe show list of possible completions
    bool showCompletions = true;
    bool onNewLine = false;
    if (lc.completionStrings.size() > completionCountCutoff) {
        int savePos = pos;  // move cursor to EOL to avoid overwriting the command line
        pos = len;
        refreshLine(pi);
        pos = savePos;
        printf("\nDisplay all %u possibilities? (y or n)",
               static_cast<unsigned int>(lc.completionStrings.size()));
        fflush(stdout);
        onNewLine = true;
        while (c != 'y' && c != 'Y' && c != 'n' && c != 'N' && c != ctrlChar('C')) {
            do {
                c = linenoiseReadChar();
                c = cleanupCtrl(c);
            } while (c == static_cast<char>(-1));
        }
        switch (c) {
            case 'n':
            case 'N':
                showCompletions = false;
                freeCompletions(&lc);
                break;
            case ctrlChar('C'):
                showCompletions = false;
                freeCompletions(&lc);
                if (write(1, "^C", 2) == -1)
                    return -1;  // Display the ^C we got
                c = 0;
                break;
        }
    }

    // if showing the list, do it the way readline does it
    bool stopList = false;
    if (showCompletions) {
        int longestCompletion = 0;
        for (size_t j = 0; j < lc.completionStrings.size(); ++j) {
            itemLength = lc.completionStrings[j].length();
            if (itemLength > longestCompletion) {
                longestCompletion = itemLength;
            }
        }
        longestCompletion += 2;
        int columnCount = pi.promptScreenColumns / longestCompletion;
        if (columnCount < 1) {
            columnCount = 1;
        }
        if (!onNewLine) {       // skip this if we showed "Display all %d possibilities?"
            int savePos = pos;  // move cursor to EOL to avoid overwriting the command line
            pos = len;
            refreshLine(pi);
            pos = savePos;
        }
        size_t pauseRow = getScreenRows() - 1;
        size_t rowCount = (lc.completionStrings.size() + columnCount - 1) / columnCount;
        for (size_t row = 0; row < rowCount; ++row) {
            if (row == pauseRow) {
                printf("\n--More--");
                fflush(stdout);
                c = 0;
                bool doBeep = false;
                while (c != ' ' && c != '\r' && c != '\n' && c != 'y' && c != 'Y' && c != 'n' &&
                       c != 'N' && c != 'q' && c != 'Q' && c != ctrlChar('C')) {
                    if (doBeep) {
                        beep();
                    }
                    doBeep = true;
                    do {
                        c = linenoiseReadChar();
                        c = cleanupCtrl(c);
                    } while (c == static_cast<char>(-1));
                }
                switch (c) {
                    case ' ':
                    case 'y':
                    case 'Y':
                        printf("\r        \r");
                        pauseRow += getScreenRows() - 1;
                        break;
                    case '\r':
                    case '\n':
                        printf("\r        \r");
                        ++pauseRow;
                        break;
                    case 'n':
                    case 'N':
                    case 'q':
                    case 'Q':
                        printf("\r        \r");
                        stopList = true;
                        break;
                    case ctrlChar('C'):
                        if (write(1, "^C", 2) == -1)
                            return -1;  // Display the ^C we got
                        stopList = true;
                        break;
                }
            } else {
                printf("\n");
            }
            if (stopList) {
                break;
            }
            for (int column = 0; column < columnCount; ++column) {
                size_t index = (column * rowCount) + row;
                if (index < lc.completionStrings.size()) {
                    itemLength = lc.completionStrings[index].length();
                    fflush(stdout);
                    if (write32(1, lc.completionStrings[index].get(), itemLength) == -1)
                        return -1;
                    if (((column + 1) * rowCount) + row < lc.completionStrings.size()) {
                        for (int k = itemLength; k < longestCompletion; ++k) {
                            printf(" ");
                        }
                    }
                }
            }
        }
        fflush(stdout);
        freeCompletions(&lc);
    }

    // display the prompt on a new line, then redisplay the input buffer
    if (!stopList || c == ctrlChar('C')) {
        if (write(1, "\n", 1) == -1)
            return 0;
    }
    if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
        return 0;
#ifndef _WIN32
    // we have to generate our own newline on line wrap on Linux
    if (pi.promptIndentation == 0 && pi.promptExtraLines > 0)
        if (write(1, "\n", 1) == -1)
            return 0;
#endif
    pi.promptCursorRowOffset = pi.promptExtraLines;
    refreshLine(pi);
    return 0;
}
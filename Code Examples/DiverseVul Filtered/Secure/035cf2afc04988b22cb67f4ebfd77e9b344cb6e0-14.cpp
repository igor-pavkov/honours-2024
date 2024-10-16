int InputBuffer::incrementalHistorySearch(PromptBase& pi, int startChar) {
    size_t bufferSize;
    size_t ucharCount;
    int errorCode;

    // if not already recalling, add the current line to the history list so we don't have to
    // special case it
    if (historyIndex == historyLen - 1) {
        free(history[historyLen - 1]);
        bufferSize = sizeof(UChar32) * len + 1;
        unique_ptr<UChar8[]> tempBuffer(new UChar8[bufferSize]);
        copyString32to8(tempBuffer.get(), buf32, bufferSize);
        history[historyLen - 1] =
            reinterpret_cast<UChar8*>(strdup(reinterpret_cast<const char*>(tempBuffer.get())));
    }
    int historyLineLength = len;
    int historyLinePosition = pos;
    UChar32 emptyBuffer[1];
    char emptyWidths[1];
    InputBuffer empty(emptyBuffer, emptyWidths, 1);
    empty.refreshLine(pi);  // erase the old input first
    DynamicPrompt dp(pi, (startChar == ctrlChar('R')) ? -1 : 1);

    dp.promptPreviousLen = pi.promptPreviousLen;
    dp.promptPreviousInputLen = pi.promptPreviousInputLen;
    dynamicRefresh(
        dp, buf32, historyLineLength, historyLinePosition);  // draw user's text with our prompt

    // loop until we get an exit character
    int c;
    bool keepLooping = true;
    bool useSearchedLine = true;
    bool searchAgain = false;
    UChar32* activeHistoryLine = 0;
    while (keepLooping) {
        c = linenoiseReadChar();
        c = cleanupCtrl(c);  // convert CTRL + <char> into normal ctrl

        switch (c) {
            // these characters keep the selected text but do not execute it
            case ctrlChar('A'):  // ctrl-A, move cursor to start of line
            case HOME_KEY:
            case ctrlChar('B'):  // ctrl-B, move cursor left by one character
            case LEFT_ARROW_KEY:
            case META + 'b':  // meta-B, move cursor left by one word
            case META + 'B':
            case CTRL + LEFT_ARROW_KEY:
            case META + LEFT_ARROW_KEY:  // Emacs allows Meta, bash & readline don't
            case ctrlChar('D'):
            case META + 'd':  // meta-D, kill word to right of cursor
            case META + 'D':
            case ctrlChar('E'):  // ctrl-E, move cursor to end of line
            case END_KEY:
            case ctrlChar('F'):  // ctrl-F, move cursor right by one character
            case RIGHT_ARROW_KEY:
            case META + 'f':  // meta-F, move cursor right by one word
            case META + 'F':
            case CTRL + RIGHT_ARROW_KEY:
            case META + RIGHT_ARROW_KEY:  // Emacs allows Meta, bash & readline don't
            case META + ctrlChar('H'):
            case ctrlChar('J'):
            case ctrlChar('K'):  // ctrl-K, kill from cursor to end of line
            case ctrlChar('M'):
            case ctrlChar('N'):  // ctrl-N, recall next line in history
            case ctrlChar('P'):  // ctrl-P, recall previous line in history
            case DOWN_ARROW_KEY:
            case UP_ARROW_KEY:
            case ctrlChar('T'):  // ctrl-T, transpose characters
            case ctrlChar('U'):  // ctrl-U, kill all characters to the left of the cursor
            case ctrlChar('W'):
            case META + 'y':  // meta-Y, "yank-pop", rotate popped text
            case META + 'Y':
            case 127:
            case DELETE_KEY:
            case META + '<':  // start of history
            case PAGE_UP_KEY:
            case META + '>':  // end of history
            case PAGE_DOWN_KEY:
                keepLooping = false;
                break;

            // these characters revert the input line to its previous state
            case ctrlChar('C'):  // ctrl-C, abort this line
            case ctrlChar('G'):
            case ctrlChar('L'):  // ctrl-L, clear screen and redisplay line
                keepLooping = false;
                useSearchedLine = false;
                if (c != ctrlChar('L')) {
                    c = -1;  // ctrl-C and ctrl-G just abort the search and do nothing else
                }
                break;

            // these characters stay in search mode and update the display
            case ctrlChar('S'):
            case ctrlChar('R'):
                if (dp.searchTextLen == 0) {  // if no current search text, recall previous text
                    if (previousSearchText.length()) {
                        dp.updateSearchText(previousSearchText.get());
                    }
                }
                if ((dp.direction == 1 && c == ctrlChar('R')) ||
                    (dp.direction == -1 && c == ctrlChar('S'))) {
                    dp.direction = 0 - dp.direction;  // reverse direction
                    dp.updateSearchPrompt();          // change the prompt
                } else {
                    searchAgain = true;  // same direction, search again
                }
                break;

// job control is its own thing
#ifndef _WIN32
            case ctrlChar('Z'):    // ctrl-Z, job control
                disableRawMode();  // Returning to Linux (whatever) shell, leave raw mode
                raise(SIGSTOP);    // Break out in mid-line
                enableRawMode();   // Back from Linux shell, re-enter raw mode
                {
                    bufferSize = historyLineLength + 1;
                    unique_ptr<UChar32[]> tempUnicode(new UChar32[bufferSize]);
                    copyString8to32(tempUnicode.get(),
                                    history[historyIndex],
                                    bufferSize,
                                    ucharCount,
                                    errorCode);
                    dynamicRefresh(dp, tempUnicode.get(), historyLineLength, historyLinePosition);
                }
                continue;
                break;
#endif

            // these characters update the search string, and hence the selected input line
            case ctrlChar('H'):  // backspace/ctrl-H, delete char to left of cursor
                if (dp.searchTextLen > 0) {
                    unique_ptr<UChar32[]> tempUnicode(new UChar32[dp.searchTextLen]);
                    --dp.searchTextLen;
                    dp.searchText[dp.searchTextLen] = 0;
                    copyString32(tempUnicode.get(), dp.searchText.get(), dp.searchTextLen + 1);
                    dp.updateSearchText(tempUnicode.get());
                } else {
                    beep();
                }
                break;

            case ctrlChar('Y'):  // ctrl-Y, yank killed text
                break;

            default:
                if (!isControlChar(c) && c <= 0x0010FFFF) {  // not an action character
                    unique_ptr<UChar32[]> tempUnicode(new UChar32[dp.searchTextLen + 2]);
                    copyString32(tempUnicode.get(), dp.searchText.get(), dp.searchTextLen + 2);
                    tempUnicode[dp.searchTextLen] = c;
                    tempUnicode[dp.searchTextLen + 1] = 0;
                    dp.updateSearchText(tempUnicode.get());
                } else {
                    beep();
                }
        }  // switch

        // if we are staying in search mode, search now
        if (keepLooping) {
            bufferSize = historyLineLength + 1;
            activeHistoryLine = new UChar32[bufferSize];
            copyString8to32(
                activeHistoryLine, history[historyIndex], bufferSize, ucharCount, errorCode);
            if (dp.searchTextLen > 0) {
                bool found = false;
                int historySearchIndex = historyIndex;
                int lineLength = ucharCount;
                int lineSearchPos = historyLinePosition;
                if (searchAgain) {
                    lineSearchPos += dp.direction;
                }
                searchAgain = false;
                while (true) {
                    while ((dp.direction > 0) ? (lineSearchPos < lineLength)
                                              : (lineSearchPos >= 0)) {
                        if (strncmp32(dp.searchText.get(),
                                      &activeHistoryLine[lineSearchPos],
                                      dp.searchTextLen) == 0) {
                            found = true;
                            break;
                        }
                        lineSearchPos += dp.direction;
                    }
                    if (found) {
                        historyIndex = historySearchIndex;
                        historyLineLength = lineLength;
                        historyLinePosition = lineSearchPos;
                        break;
                    } else if ((dp.direction > 0) ? (historySearchIndex < historyLen - 1)
                                                  : (historySearchIndex > 0)) {
                        historySearchIndex += dp.direction;
                        bufferSize =
                            strlen(reinterpret_cast<char*>(history[historySearchIndex])) + 1;
                        delete[] activeHistoryLine;
                        activeHistoryLine = new UChar32[bufferSize];
                        copyString8to32(activeHistoryLine,
                                        history[historySearchIndex],
                                        bufferSize,
                                        ucharCount,
                                        errorCode);
                        lineLength = ucharCount;
                        lineSearchPos = (dp.direction > 0) ? 0 : (lineLength - dp.searchTextLen);
                    } else {
                        beep();
                        break;
                    }
                };  // while
            }
            if (activeHistoryLine) {
                delete[] activeHistoryLine;
            }
            bufferSize = historyLineLength + 1;
            activeHistoryLine = new UChar32[bufferSize];
            copyString8to32(
                activeHistoryLine, history[historyIndex], bufferSize, ucharCount, errorCode);
            dynamicRefresh(dp,
                           activeHistoryLine,
                           historyLineLength,
                           historyLinePosition);  // draw user's text with our prompt
        }
    }  // while

    // leaving history search, restore previous prompt, maybe make searched line current
    PromptBase pb;
    pb.promptChars = pi.promptIndentation;
    Utf32String tempUnicode(pb.promptChars + 1);
    copyString32(tempUnicode.get(), &pi.promptText[pi.promptLastLinePosition], pb.promptChars + 1);
    tempUnicode.initFromBuffer();
    pb.promptText = tempUnicode;
    pb.promptExtraLines = 0;
    pb.promptIndentation = pi.promptIndentation;
    pb.promptLastLinePosition = 0;
    pb.promptPreviousInputLen = historyLineLength;
    pb.promptCursorRowOffset = dp.promptCursorRowOffset;
    pb.promptScreenColumns = pi.promptScreenColumns;
    pb.promptPreviousLen = dp.promptChars;
    if (useSearchedLine && activeHistoryLine) {
        historyRecallMostRecent = true;
        copyString32(buf32, activeHistoryLine, buflen + 1);
        len = historyLineLength;
        pos = historyLinePosition;
    }
    if (activeHistoryLine) {
        delete[] activeHistoryLine;
    }
    dynamicRefresh(pb, buf32, len, pos);  // redraw the original prompt with current input
    pi.promptPreviousInputLen = len;
    pi.promptCursorRowOffset = pi.promptExtraLines + pb.promptCursorRowOffset;
    previousSearchText = dp.searchText;  // save search text for possible reuse on ctrl-R ctrl-R
    return c;                            // pass a character or -1 back to main loop
}
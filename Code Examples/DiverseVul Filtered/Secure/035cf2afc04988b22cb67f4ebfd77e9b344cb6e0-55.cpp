int InputBuffer::getInputLine(PromptBase& pi) {
    // The latest history entry is always our current buffer
    if (len > 0) {
        size_t bufferSize = sizeof(UChar32) * len + 1;
        unique_ptr<char[]> tempBuffer(new char[bufferSize]);
        copyString32to8(reinterpret_cast<UChar8*>(tempBuffer.get()), buf32, bufferSize);
        linenoiseHistoryAdd(tempBuffer.get());
    } else {
        linenoiseHistoryAdd("");
    }
    historyIndex = historyLen - 1;
    historyRecallMostRecent = false;

    // display the prompt
    if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
        return -1;

#ifndef _WIN32
    // we have to generate our own newline on line wrap on Linux
    if (pi.promptIndentation == 0 && pi.promptExtraLines > 0)
        if (write(1, "\n", 1) == -1)
            return -1;
#endif

    // the cursor starts out at the end of the prompt
    pi.promptCursorRowOffset = pi.promptExtraLines;

    // kill and yank start in "other" mode
    killRing.lastAction = KillRing::actionOther;

    // when history search returns control to us, we execute its terminating keystroke
    int terminatingKeystroke = -1;

    // if there is already text in the buffer, display it first
    if (len > 0) {
        refreshLine(pi);
    }

    // loop collecting characters, respond to line editing characters
    while (true) {
        int c;
        if (terminatingKeystroke == -1) {
            c = linenoiseReadChar();  // get a new keystroke
        } else {
            c = terminatingKeystroke;   // use the terminating keystroke from search
            terminatingKeystroke = -1;  // clear it once we've used it
        }
        c = cleanupCtrl(c);  // convert CTRL + <char> into normal ctrl

        if (c == 0) {
            return len;
        }

        if (c == -1) {
            refreshLine(pi);
            continue;
        }

        if (c == -2) {
            if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
                return -1;
            refreshLine(pi);
            continue;
        }

        // ctrl-I/tab, command completion, needs to be before switch statement
        if (c == ctrlChar('I') && completionCallback) {
            if (pos == 0)  // SERVER-4967 -- in earlier versions, you could paste previous output
                continue;  //  back into the shell ... this output may have leading tabs.
                           // This hack (i.e. what the old code did) prevents command completion
                           //  on an empty line but lets users paste text with leading tabs.

            killRing.lastAction = KillRing::actionOther;
            historyRecallMostRecent = false;

            // completeLine does the actual completion and replacement
            c = completeLine(pi);

            if (c < 0)  // return on error
                return len;

            if (c == 0)  // read next character when 0
                continue;

            // deliberate fall-through here, so we use the terminating character
        }

        switch (c) {
            case ctrlChar('A'):  // ctrl-A, move cursor to start of line
            case HOME_KEY:
                killRing.lastAction = KillRing::actionOther;
                pos = 0;
                refreshLine(pi);
                break;

            case ctrlChar('B'):  // ctrl-B, move cursor left by one character
            case LEFT_ARROW_KEY:
                killRing.lastAction = KillRing::actionOther;
                if (pos > 0) {
                    --pos;
                    refreshLine(pi);
                }
                break;

            case META + 'b':  // meta-B, move cursor left by one word
            case META + 'B':
            case CTRL + LEFT_ARROW_KEY:
            case META + LEFT_ARROW_KEY:  // Emacs allows Meta, bash & readline don't
                killRing.lastAction = KillRing::actionOther;
                if (pos > 0) {
                    while (pos > 0 && !isCharacterAlphanumeric(buf32[pos - 1])) {
                        --pos;
                    }
                    while (pos > 0 && isCharacterAlphanumeric(buf32[pos - 1])) {
                        --pos;
                    }
                    refreshLine(pi);
                }
                break;

            case ctrlChar('C'):  // ctrl-C, abort this line
                killRing.lastAction = KillRing::actionOther;
                historyRecallMostRecent = false;
                errno = EAGAIN;
                --historyLen;
                free(history[historyLen]);
                // we need one last refresh with the cursor at the end of the line
                // so we don't display the next prompt over the previous input line
                pos = len;  // pass len as pos for EOL
                refreshLine(pi);
                if (write(1, "^C", 2) == -1)
                    return -1;  // Display the ^C we got
                return -1;

            case META + 'c':  // meta-C, give word initial Cap
            case META + 'C':
                killRing.lastAction = KillRing::actionOther;
                historyRecallMostRecent = false;
                if (pos < len) {
                    while (pos < len && !isCharacterAlphanumeric(buf32[pos])) {
                        ++pos;
                    }
                    if (pos < len && isCharacterAlphanumeric(buf32[pos])) {
                        if (buf32[pos] >= 'a' && buf32[pos] <= 'z') {
                            buf32[pos] += 'A' - 'a';
                        }
                        ++pos;
                    }
                    while (pos < len && isCharacterAlphanumeric(buf32[pos])) {
                        if (buf32[pos] >= 'A' && buf32[pos] <= 'Z') {
                            buf32[pos] += 'a' - 'A';
                        }
                        ++pos;
                    }
                    refreshLine(pi);
                }
                break;

            // ctrl-D, delete the character under the cursor
            // on an empty line, exit the shell
            case ctrlChar('D'):
                killRing.lastAction = KillRing::actionOther;
                if (len > 0 && pos < len) {
                    historyRecallMostRecent = false;
                    memmove(buf32 + pos, buf32 + pos + 1, sizeof(UChar32) * (len - pos));
                    --len;
                    refreshLine(pi);
                } else if (len == 0) {
                    --historyLen;
                    free(history[historyLen]);
                    return -1;
                }
                break;

            case META + 'd':  // meta-D, kill word to right of cursor
            case META + 'D':
                if (pos < len) {
                    historyRecallMostRecent = false;
                    int endingPos = pos;
                    while (endingPos < len && !isCharacterAlphanumeric(buf32[endingPos])) {
                        ++endingPos;
                    }
                    while (endingPos < len && isCharacterAlphanumeric(buf32[endingPos])) {
                        ++endingPos;
                    }
                    killRing.kill(&buf32[pos], endingPos - pos, true);
                    memmove(
                        buf32 + pos, buf32 + endingPos, sizeof(UChar32) * (len - endingPos + 1));
                    len -= endingPos - pos;
                    refreshLine(pi);
                }
                killRing.lastAction = KillRing::actionKill;
                break;

            case ctrlChar('E'):  // ctrl-E, move cursor to end of line
            case END_KEY:
                killRing.lastAction = KillRing::actionOther;
                pos = len;
                refreshLine(pi);
                break;

            case ctrlChar('F'):  // ctrl-F, move cursor right by one character
            case RIGHT_ARROW_KEY:
                killRing.lastAction = KillRing::actionOther;
                if (pos < len) {
                    ++pos;
                    refreshLine(pi);
                }
                break;

            case META + 'f':  // meta-F, move cursor right by one word
            case META + 'F':
            case CTRL + RIGHT_ARROW_KEY:
            case META + RIGHT_ARROW_KEY:  // Emacs allows Meta, bash & readline don't
                killRing.lastAction = KillRing::actionOther;
                if (pos < len) {
                    while (pos < len && !isCharacterAlphanumeric(buf32[pos])) {
                        ++pos;
                    }
                    while (pos < len && isCharacterAlphanumeric(buf32[pos])) {
                        ++pos;
                    }
                    refreshLine(pi);
                }
                break;

            case ctrlChar('H'):  // backspace/ctrl-H, delete char to left of cursor
                killRing.lastAction = KillRing::actionOther;
                if (pos > 0) {
                    historyRecallMostRecent = false;
                    memmove(buf32 + pos - 1, buf32 + pos, sizeof(UChar32) * (1 + len - pos));
                    --pos;
                    --len;
                    refreshLine(pi);
                }
                break;

            // meta-Backspace, kill word to left of cursor
            case META + ctrlChar('H'):
                if (pos > 0) {
                    historyRecallMostRecent = false;
                    int startingPos = pos;
                    while (pos > 0 && !isCharacterAlphanumeric(buf32[pos - 1])) {
                        --pos;
                    }
                    while (pos > 0 && isCharacterAlphanumeric(buf32[pos - 1])) {
                        --pos;
                    }
                    killRing.kill(&buf32[pos], startingPos - pos, false);
                    memmove(buf32 + pos,
                            buf32 + startingPos,
                            sizeof(UChar32) * (len - startingPos + 1));
                    len -= startingPos - pos;
                    refreshLine(pi);
                }
                killRing.lastAction = KillRing::actionKill;
                break;

            case ctrlChar('J'):  // ctrl-J/linefeed/newline, accept line
            case ctrlChar('M'):  // ctrl-M/return/enter
                killRing.lastAction = KillRing::actionOther;
                // we need one last refresh with the cursor at the end of the line
                // so we don't display the next prompt over the previous input line
                pos = len;  // pass len as pos for EOL
                refreshLine(pi);
                historyPreviousIndex = historyRecallMostRecent ? historyIndex : -2;
                --historyLen;
                free(history[historyLen]);
                return len;

            case ctrlChar('K'):  // ctrl-K, kill from cursor to end of line
                killRing.kill(&buf32[pos], len - pos, true);
                buf32[pos] = '\0';
                len = pos;
                refreshLine(pi);
                killRing.lastAction = KillRing::actionKill;
                historyRecallMostRecent = false;
                break;

            case ctrlChar('L'):  // ctrl-L, clear screen and redisplay line
                clearScreen(pi);
                break;

            case META + 'l':  // meta-L, lowercase word
            case META + 'L':
                killRing.lastAction = KillRing::actionOther;
                if (pos < len) {
                    historyRecallMostRecent = false;
                    while (pos < len && !isCharacterAlphanumeric(buf32[pos])) {
                        ++pos;
                    }
                    while (pos < len && isCharacterAlphanumeric(buf32[pos])) {
                        if (buf32[pos] >= 'A' && buf32[pos] <= 'Z') {
                            buf32[pos] += 'a' - 'A';
                        }
                        ++pos;
                    }
                    refreshLine(pi);
                }
                break;

            case ctrlChar('N'):  // ctrl-N, recall next line in history
            case ctrlChar('P'):  // ctrl-P, recall previous line in history
            case DOWN_ARROW_KEY:
            case UP_ARROW_KEY:
                killRing.lastAction = KillRing::actionOther;
                // if not already recalling, add the current line to the history list so we don't
                // have to special case it
                if (historyIndex == historyLen - 1) {
                    free(history[historyLen - 1]);
                    size_t tempBufferSize = sizeof(UChar32) * len + 1;
                    unique_ptr<UChar8[]> tempBuffer(new UChar8[tempBufferSize]);
                    copyString32to8(tempBuffer.get(), buf32, tempBufferSize);
                    history[historyLen - 1] = reinterpret_cast<UChar8*>(
                        strdup(reinterpret_cast<const char*>(tempBuffer.get())));
                }
                if (historyLen > 1) {
                    if (c == UP_ARROW_KEY) {
                        c = ctrlChar('P');
                    }
                    if (historyPreviousIndex != -2 && c != ctrlChar('P')) {
                        historyIndex = 1 + historyPreviousIndex;  // emulate Windows down-arrow
                    } else {
                        historyIndex += (c == ctrlChar('P')) ? -1 : 1;
                    }
                    historyPreviousIndex = -2;
                    if (historyIndex < 0) {
                        historyIndex = 0;
                        break;
                    } else if (historyIndex >= historyLen) {
                        historyIndex = historyLen - 1;
                        break;
                    }
                    historyRecallMostRecent = true;
                    size_t ucharCount;
                    int errorCode;
                    copyString8to32(buf32, history[historyIndex], buflen, ucharCount, errorCode);
                    len = pos = ucharCount;
                    refreshLine(pi);
                }
                break;

            case ctrlChar('R'):  // ctrl-R, reverse history search
            case ctrlChar('S'):  // ctrl-S, forward history search
                terminatingKeystroke = incrementalHistorySearch(pi, c);
                break;

            case ctrlChar('T'):  // ctrl-T, transpose characters
                killRing.lastAction = KillRing::actionOther;
                if (pos > 0 && len > 1) {
                    historyRecallMostRecent = false;
                    size_t leftCharPos = (pos == len) ? pos - 2 : pos - 1;
                    char aux = buf32[leftCharPos];
                    buf32[leftCharPos] = buf32[leftCharPos + 1];
                    buf32[leftCharPos + 1] = aux;
                    if (pos != len)
                        ++pos;
                    refreshLine(pi);
                }
                break;

            case ctrlChar('U'):  // ctrl-U, kill all characters to the left of the cursor
                if (pos > 0) {
                    historyRecallMostRecent = false;
                    killRing.kill(&buf32[0], pos, false);
                    len -= pos;
                    memmove(buf32, buf32 + pos, sizeof(UChar32) * (len + 1));
                    pos = 0;
                    refreshLine(pi);
                }
                killRing.lastAction = KillRing::actionKill;
                break;

            case META + 'u':  // meta-U, uppercase word
            case META + 'U':
                killRing.lastAction = KillRing::actionOther;
                if (pos < len) {
                    historyRecallMostRecent = false;
                    while (pos < len && !isCharacterAlphanumeric(buf32[pos])) {
                        ++pos;
                    }
                    while (pos < len && isCharacterAlphanumeric(buf32[pos])) {
                        if (buf32[pos] >= 'a' && buf32[pos] <= 'z') {
                            buf32[pos] += 'A' - 'a';
                        }
                        ++pos;
                    }
                    refreshLine(pi);
                }
                break;

            // ctrl-W, kill to whitespace (not word) to left of cursor
            case ctrlChar('W'):
                if (pos > 0) {
                    historyRecallMostRecent = false;
                    int startingPos = pos;
                    while (pos > 0 && buf32[pos - 1] == ' ') {
                        --pos;
                    }
                    while (pos > 0 && buf32[pos - 1] != ' ') {
                        --pos;
                    }
                    killRing.kill(&buf32[pos], startingPos - pos, false);
                    memmove(buf32 + pos,
                            buf32 + startingPos,
                            sizeof(UChar32) * (len - startingPos + 1));
                    len -= startingPos - pos;
                    refreshLine(pi);
                }
                killRing.lastAction = KillRing::actionKill;
                break;

            case ctrlChar('Y'):  // ctrl-Y, yank killed text
                historyRecallMostRecent = false;
                {
                    Utf32String* restoredText = killRing.yank();
                    if (restoredText) {
                        bool truncated = false;
                        size_t ucharCount = restoredText->length();
                        if (ucharCount > static_cast<size_t>(buflen - len)) {
                            ucharCount = buflen - len;
                            truncated = true;
                        }
                        memmove(buf32 + pos + ucharCount,
                                buf32 + pos,
                                sizeof(UChar32) * (len - pos + 1));
                        memmove(buf32 + pos, restoredText->get(), sizeof(UChar32) * ucharCount);
                        pos += ucharCount;
                        len += ucharCount;
                        refreshLine(pi);
                        killRing.lastAction = KillRing::actionYank;
                        killRing.lastYankSize = ucharCount;
                        if (truncated) {
                            beep();
                        }
                    } else {
                        beep();
                    }
                }
                break;

            case META + 'y':  // meta-Y, "yank-pop", rotate popped text
            case META + 'Y':
                if (killRing.lastAction == KillRing::actionYank) {
                    historyRecallMostRecent = false;
                    Utf32String* restoredText = killRing.yankPop();
                    if (restoredText) {
                        bool truncated = false;
                        size_t ucharCount = restoredText->length();
                        if (ucharCount >
                            static_cast<size_t>(killRing.lastYankSize + buflen - len)) {
                            ucharCount = killRing.lastYankSize + buflen - len;
                            truncated = true;
                        }
                        if (ucharCount > killRing.lastYankSize) {
                            memmove(buf32 + pos + ucharCount - killRing.lastYankSize,
                                    buf32 + pos,
                                    sizeof(UChar32) * (len - pos + 1));
                            memmove(buf32 + pos - killRing.lastYankSize,
                                    restoredText->get(),
                                    sizeof(UChar32) * ucharCount);
                        } else {
                            memmove(buf32 + pos - killRing.lastYankSize,
                                    restoredText->get(),
                                    sizeof(UChar32) * ucharCount);
                            memmove(buf32 + pos + ucharCount - killRing.lastYankSize,
                                    buf32 + pos,
                                    sizeof(UChar32) * (len - pos + 1));
                        }
                        pos += ucharCount - killRing.lastYankSize;
                        len += ucharCount - killRing.lastYankSize;
                        killRing.lastYankSize = ucharCount;
                        refreshLine(pi);
                        if (truncated) {
                            beep();
                        }
                        break;
                    }
                }
                beep();
                break;

#ifndef _WIN32
            case ctrlChar('Z'):    // ctrl-Z, job control
                disableRawMode();  // Returning to Linux (whatever) shell, leave raw mode
                raise(SIGSTOP);    // Break out in mid-line
                enableRawMode();   // Back from Linux shell, re-enter raw mode
                if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
                    break;        // Redraw prompt
                refreshLine(pi);  // Refresh the line
                break;
#endif

            // DEL, delete the character under the cursor
            case 127:
            case DELETE_KEY:
                killRing.lastAction = KillRing::actionOther;
                if (len > 0 && pos < len) {
                    historyRecallMostRecent = false;
                    memmove(buf32 + pos, buf32 + pos + 1, sizeof(UChar32) * (len - pos));
                    --len;
                    refreshLine(pi);
                }
                break;

            case META + '<':     // meta-<, beginning of history
            case PAGE_UP_KEY:    // Page Up, beginning of history
            case META + '>':     // meta->, end of history
            case PAGE_DOWN_KEY:  // Page Down, end of history
                killRing.lastAction = KillRing::actionOther;
                // if not already recalling, add the current line to the history list so we don't
                // have to special case it
                if (historyIndex == historyLen - 1) {
                    free(history[historyLen - 1]);
                    size_t tempBufferSize = sizeof(UChar32) * len + 1;
                    unique_ptr<UChar8[]> tempBuffer(new UChar8[tempBufferSize]);
                    copyString32to8(tempBuffer.get(), buf32, tempBufferSize);
                    history[historyLen - 1] = reinterpret_cast<UChar8*>(
                        strdup(reinterpret_cast<const char*>(tempBuffer.get())));
                }
                if (historyLen > 1) {
                    historyIndex = (c == META + '<' || c == PAGE_UP_KEY) ? 0 : historyLen - 1;
                    historyPreviousIndex = -2;
                    historyRecallMostRecent = true;
                    size_t ucharCount;
                    int errorCode;
                    copyString8to32(buf32, history[historyIndex], buflen, ucharCount, errorCode);
                    len = pos = ucharCount;
                    refreshLine(pi);
                }
                break;

            // not one of our special characters, maybe insert it in the buffer
            default:
                killRing.lastAction = KillRing::actionOther;
                historyRecallMostRecent = false;
                if (c & (META | CTRL)) {  // beep on unknown Ctrl and/or Meta keys
                    beep();
                    break;
                }
                if (len < buflen) {
                    if (isControlChar(c)) {  // don't insert control characters
                        beep();
                        break;
                    }
                    if (len == pos) {  // at end of buffer
                        buf32[pos] = c;
                        ++pos;
                        ++len;
                        buf32[len] = '\0';
                        int inputLen = calculateColumnPosition(buf32, len);
                        if (pi.promptIndentation + inputLen < pi.promptScreenColumns) {
                            if (inputLen > pi.promptPreviousInputLen)
                                pi.promptPreviousInputLen = inputLen;
                            /* Avoid a full update of the line in the
                             * trivial case. */
                            if (write32(1, reinterpret_cast<UChar32*>(&c), 1) == -1)
                                return -1;
                        } else {
                            refreshLine(pi);
                        }
                    } else {  // not at end of buffer, have to move characters to our right
                        memmove(buf32 + pos + 1, buf32 + pos, sizeof(UChar32) * (len - pos));
                        buf32[pos] = c;
                        ++len;
                        ++pos;
                        buf32[len] = '\0';
                        refreshLine(pi);
                    }
                } else {
                    beep();  // buffer is full, beep on new characters
                }
                break;
        }
    }
    return len;
}
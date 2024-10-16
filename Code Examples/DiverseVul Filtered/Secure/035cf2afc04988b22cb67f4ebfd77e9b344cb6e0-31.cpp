static void dynamicRefresh(PromptBase& pi, UChar32* buf32, int len, int pos) {
    // calculate the position of the end of the prompt
    int xEndOfPrompt, yEndOfPrompt;
    calculateScreenPosition(
        0, 0, pi.promptScreenColumns, pi.promptChars, xEndOfPrompt, yEndOfPrompt);
    pi.promptIndentation = xEndOfPrompt;

    // calculate the position of the end of the input line
    int xEndOfInput, yEndOfInput;
    calculateScreenPosition(xEndOfPrompt,
                            yEndOfPrompt,
                            pi.promptScreenColumns,
                            calculateColumnPosition(buf32, len),
                            xEndOfInput,
                            yEndOfInput);

    // calculate the desired position of the cursor
    int xCursorPos, yCursorPos;
    calculateScreenPosition(xEndOfPrompt,
                            yEndOfPrompt,
                            pi.promptScreenColumns,
                            calculateColumnPosition(buf32, pos),
                            xCursorPos,
                            yCursorPos);

#ifdef _WIN32
    // position at the start of the prompt, clear to end of previous input
    CONSOLE_SCREEN_BUFFER_INFO inf;
    GetConsoleScreenBufferInfo(console_out, &inf);
    inf.dwCursorPosition.X = 0;
    inf.dwCursorPosition.Y -= pi.promptCursorRowOffset /*- pi.promptExtraLines*/;
    SetConsoleCursorPosition(console_out, inf.dwCursorPosition);
    DWORD count;
    FillConsoleOutputCharacterA(console_out,
                                ' ',
                                pi.promptPreviousLen + pi.promptPreviousInputLen,
                                inf.dwCursorPosition,
                                &count);
    pi.promptPreviousLen = pi.promptIndentation;
    pi.promptPreviousInputLen = len;

    // display the prompt
    if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
        return;

    // display the input line
    if (write32(1, buf32, len) == -1)
        return;

    // position the cursor
    GetConsoleScreenBufferInfo(console_out, &inf);
    inf.dwCursorPosition.X = xCursorPos;  // 0-based on Win32
    inf.dwCursorPosition.Y -= yEndOfInput - yCursorPos;
    SetConsoleCursorPosition(console_out, inf.dwCursorPosition);
#else  // _WIN32
    char seq[64];
    int cursorRowMovement = pi.promptCursorRowOffset - pi.promptExtraLines;
    if (cursorRowMovement > 0) {  // move the cursor up as required
        snprintf(seq, sizeof seq, "\x1b[%dA", cursorRowMovement);
        if (write(1, seq, strlen(seq)) == -1)
            return;
    }
    // position at the start of the prompt, clear to end of screen
    snprintf(seq, sizeof seq, "\x1b[1G\x1b[J");  // 1-based on VT100
    if (write(1, seq, strlen(seq)) == -1)
        return;

    // display the prompt
    if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
        return;

    // display the input line
    if (write32(1, buf32, len) == -1)
        return;

    // we have to generate our own newline on line wrap
    if (xEndOfInput == 0 && yEndOfInput > 0)
        if (write(1, "\n", 1) == -1)
            return;

    // position the cursor
    cursorRowMovement = yEndOfInput - yCursorPos;
    if (cursorRowMovement > 0) {  // move the cursor up as required
        snprintf(seq, sizeof seq, "\x1b[%dA", cursorRowMovement);
        if (write(1, seq, strlen(seq)) == -1)
            return;
    }
    // position the cursor within the line
    snprintf(seq, sizeof seq, "\x1b[%dG", xCursorPos + 1);  // 1-based on VT100
    if (write(1, seq, strlen(seq)) == -1)
        return;
#endif

    pi.promptCursorRowOffset = pi.promptExtraLines + yCursorPos;  // remember row for next pass
}
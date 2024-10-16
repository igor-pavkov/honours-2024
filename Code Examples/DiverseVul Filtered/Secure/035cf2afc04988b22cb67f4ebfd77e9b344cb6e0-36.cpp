void InputBuffer::clearScreen(PromptBase& pi) {
    linenoiseClearScreen();
    if (write32(1, pi.promptText.get(), pi.promptChars) == -1)
        return;
#ifndef _WIN32
    // we have to generate our own newline on line wrap on Linux
    if (pi.promptIndentation == 0 && pi.promptExtraLines > 0)
        if (write(1, "\n", 1) == -1)
            return;
#endif
    pi.promptCursorRowOffset = pi.promptExtraLines;
    refreshLine(pi);
}
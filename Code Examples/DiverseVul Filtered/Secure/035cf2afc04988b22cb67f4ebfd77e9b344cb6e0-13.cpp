static void setDisplayAttribute(bool enhancedDisplay) {
#ifdef _WIN32
    if (enhancedDisplay) {
        CONSOLE_SCREEN_BUFFER_INFO inf;
        GetConsoleScreenBufferInfo(console_out, &inf);
        oldDisplayAttribute = inf.wAttributes;
        BYTE oldLowByte = oldDisplayAttribute & 0xFF;
        BYTE newLowByte;
        switch (oldLowByte) {
            case 0x07:
                // newLowByte = FOREGROUND_BLUE | FOREGROUND_INTENSITY;  // too dim
                // newLowByte = FOREGROUND_BLUE;                         // even dimmer
                newLowByte =
                    FOREGROUND_BLUE | FOREGROUND_GREEN;  // most similar to xterm appearance
                break;
            case 0x70:
                newLowByte = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
                break;
            default:
                newLowByte = oldLowByte ^ 0xFF;  // default to inverse video
                break;
        }
        inf.wAttributes = (inf.wAttributes & 0xFF00) | newLowByte;
        SetConsoleTextAttribute(console_out, inf.wAttributes);
    } else {
        SetConsoleTextAttribute(console_out, oldDisplayAttribute);
    }
#else
    if (enhancedDisplay) {
        if (write(1, "\x1b[1;34m", 7) == -1)
            return; /* bright blue (visible with both B&W bg) */
    } else {
        if (write(1, "\x1b[0m", 4) == -1)
            return; /* reset */
    }
#endif
}
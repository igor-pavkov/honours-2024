    InputBuffer(UChar32* buffer, char* widthArray, int bufferLen)
        : buf32(buffer), charWidths(widthArray), buflen(bufferLen - 1), len(0), pos(0) {
        buf32[0] = 0;
    }
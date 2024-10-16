static int getScreenRows(void) {
    int rows;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO inf;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf);
    rows = 1 + inf.srWindow.Bottom - inf.srWindow.Top;
#else
    struct winsize ws;
    rows = (ioctl(1, TIOCGWINSZ, &ws) == -1) ? 24 : ws.ws_row;
#endif
    return (rows > 0) ? rows : 24;
}
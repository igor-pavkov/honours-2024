static int getScreenColumns(void) {
    int cols;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO inf;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &inf);
    cols = inf.dwSize.X;
#else
    struct winsize ws;
    cols = (ioctl(1, TIOCGWINSZ, &ws) == -1) ? 80 : ws.ws_col;
#endif
    // cols is 0 in certain circumstances like inside debugger, which creates further issues
    return (cols > 0) ? cols : 80;
}
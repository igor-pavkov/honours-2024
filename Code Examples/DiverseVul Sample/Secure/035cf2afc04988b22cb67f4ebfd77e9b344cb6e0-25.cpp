void linenoiseClearScreen(void) {
#ifdef _WIN32
    COORD coord = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO inf;
    HANDLE screenHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(screenHandle, &inf);
    SetConsoleCursorPosition(screenHandle, coord);
    DWORD count;
    FillConsoleOutputCharacterA(screenHandle, ' ', inf.dwSize.X * inf.dwSize.Y, coord, &count);
#else
    if (write(1, "\x1b[H\x1b[2J", 7) <= 0)
        return;
#endif
}
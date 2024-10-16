static void disableRawMode(void) {
#ifdef _WIN32
    SetConsoleMode(console_in, oldMode);
    console_in = 0;
    console_out = 0;
#else
    if (rawmode && tcsetattr(0, TCSADRAIN, &orig_termios) != -1)
        rawmode = 0;
#endif
}
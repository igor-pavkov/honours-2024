static int cleanupCtrl(int c) {
    if (c & CTRL) {
        int d = c & 0x1FF;
        if (d >= 'a' && d <= 'z') {
            c = (c + ('a' - ctrlChar('A'))) & ~CTRL;
        }
        if (d >= 'A' && d <= 'Z') {
            c = (c + ('A' - ctrlChar('A'))) & ~CTRL;
        }
        if (d >= ctrlChar('A') && d <= ctrlChar('Z')) {
            c = c & ~CTRL;
        }
    }
    return c;
}
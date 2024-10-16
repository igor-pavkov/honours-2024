static void beep() {
    fprintf(stderr, "\x7");  // ctrl-G == bell/beep
    fflush(stderr);
}
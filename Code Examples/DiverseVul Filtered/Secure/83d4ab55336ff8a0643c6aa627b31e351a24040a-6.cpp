static bool display_is_local(const char *display) {
        assert(display);

        return
                display[0] == ':' &&
                display[1] >= '0' &&
                display[1] <= '9';
}
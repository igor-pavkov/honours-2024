static unsigned int gif_interlaced_line(int height, int y) {
        if ((y << 3) < height) {
                return (y << 3);
        }
        y -= ((height + 7) >> 3);
        if ((y << 3) < (height - 4)) {
                return (y << 3) + 4;
        }
        y -= ((height + 3) >> 3);
        if ((y << 2) < (height - 2)) {
                return (y << 2) + 2;
        }
        y -= ((height + 1) >> 2);
        return (y << 1) + 1;
}
static unsigned int rgb_to_pixel15bgr_dup(unsigned int r, unsigned int g,
                                          unsigned int b)
{
    unsigned int col;
    col = rgb_to_pixel15bgr(r, g, b);
    col |= col << 16;
    return col;
}
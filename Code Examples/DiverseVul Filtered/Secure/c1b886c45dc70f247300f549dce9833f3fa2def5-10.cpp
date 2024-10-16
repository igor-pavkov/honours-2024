static unsigned int rgb_to_pixel32bgr_dup(unsigned int r, unsigned int g, unsigned b)
{
    unsigned int col;
    col = rgb_to_pixel32bgr(r, g, b);
    return col;
}
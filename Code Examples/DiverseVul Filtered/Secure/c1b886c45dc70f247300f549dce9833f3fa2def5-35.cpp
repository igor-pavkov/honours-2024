static inline int get_depth_index(DisplaySurface *s)
{
    switch (surface_bits_per_pixel(s)) {
    default:
    case 8:
        return 0;
    case 15:
        return 1;
    case 16:
        return 2;
    case 32:
        if (is_surface_bgr(s)) {
            return 4;
        } else {
            return 3;
        }
    }
}
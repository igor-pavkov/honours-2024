static inline int int_clamp(int val, int vmin, int vmax)
{
    if (val < vmin) {
        return vmin;
    } else if (val > vmax) {
        return vmax;
    } else {
        return val;
    }
}
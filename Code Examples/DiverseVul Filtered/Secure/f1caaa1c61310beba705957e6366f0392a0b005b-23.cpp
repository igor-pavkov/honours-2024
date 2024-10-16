static inline int get_bucket(int value, int shift)
{
    value >>= shift;
    value &= NBUCKETS - 1;
    return NBUCKETS - 1 - value;
}
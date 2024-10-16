static inline float reminder(float v, int x)
{
    return ((v / x) - floor(v / x)) * x;
}
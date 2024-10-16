static inline unsigned exp_random(double mean)
{
    return -mean * log((double)rand() / RAND_MAX);
}
static void radix_count(const RCCMPEntry *data, int size, int buckets[RADIX_PASSES][NBUCKETS])
{
    int i, j;
    memset(buckets, 0, sizeof(buckets[0][0]) * RADIX_PASSES * NBUCKETS);
    for (i = 0; i < size; i++) {
        int v = data[i].value;
        for (j = 0; j < RADIX_PASSES; j++) {
            buckets[j][get_bucket(v, 0)]++;
            v >>= BUCKET_BITS;
        }
        av_assert1(!v);
    }
    for (j = 0; j < RADIX_PASSES; j++) {
        int offset = size;
        for (i = NBUCKETS - 1; i >= 0; i--)
            buckets[j][i] = offset -= buckets[j][i];
        av_assert1(!buckets[j][0]);
    }
}
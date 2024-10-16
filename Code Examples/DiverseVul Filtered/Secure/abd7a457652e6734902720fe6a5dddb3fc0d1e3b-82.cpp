static unsigned hash_to_bucket(uint32_t hash)
{
    /* Extracts the most significant bits in hash. The least significant bits
     * are already used internally by the hmap implementation. */
    BUILD_ASSERT(CONNTRACK_BUCKETS_SHIFT < 32 && CONNTRACK_BUCKETS_SHIFT >= 1);

    return (hash >> (32 - CONNTRACK_BUCKETS_SHIFT)) % CONNTRACK_BUCKETS;
}
static inline void dp_packet_batch_init(struct dp_packet_batch *b)
{
    b->count = 0;
    b->trunc = false;
}
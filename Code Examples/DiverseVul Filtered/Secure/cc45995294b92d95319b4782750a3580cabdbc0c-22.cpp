static inline uint16_t vring_used_event(VirtQueue *vq)
{
    return vring_avail_ring(vq, vq->vring.num);
}
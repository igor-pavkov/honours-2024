static inline uint16_t vring_avail_ring(VirtQueue *vq, int i)
{
    hwaddr pa;
    pa = vq->vring.avail + offsetof(VRingAvail, ring[i]);
    return lduw_phys(&address_space_memory, pa);
}
static inline uint16_t vring_avail_idx(VirtQueue *vq)
{
    hwaddr pa;
    pa = vq->vring.avail + offsetof(VRingAvail, idx);
    return lduw_phys(&address_space_memory, pa);
}
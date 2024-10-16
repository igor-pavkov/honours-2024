static inline void vring_avail_event(VirtQueue *vq, uint16_t val)
{
    hwaddr pa;
    if (!vq->notification) {
        return;
    }
    pa = vq->vring.used + offsetof(VRingUsed, ring[vq->vring.num]);
    stw_phys(&address_space_memory, pa, val);
}
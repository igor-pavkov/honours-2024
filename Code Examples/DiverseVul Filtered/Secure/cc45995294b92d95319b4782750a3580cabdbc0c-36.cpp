static inline void vring_used_ring_id(VirtQueue *vq, int i, uint32_t val)
{
    hwaddr pa;
    pa = vq->vring.used + offsetof(VRingUsed, ring[i].id);
    stl_phys(&address_space_memory, pa, val);
}
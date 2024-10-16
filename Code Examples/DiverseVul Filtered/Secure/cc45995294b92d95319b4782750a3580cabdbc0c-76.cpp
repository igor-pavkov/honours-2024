static inline void vring_used_idx_set(VirtQueue *vq, uint16_t val)
{
    hwaddr pa;
    pa = vq->vring.used + offsetof(VRingUsed, idx);
    stw_phys(&address_space_memory, pa, val);
}
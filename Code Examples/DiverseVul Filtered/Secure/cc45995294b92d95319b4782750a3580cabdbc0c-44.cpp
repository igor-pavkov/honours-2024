static inline void vring_used_flags_unset_bit(VirtQueue *vq, int mask)
{
    hwaddr pa;
    pa = vq->vring.used + offsetof(VRingUsed, flags);
    stw_phys(&address_space_memory,
             pa, lduw_phys(&address_space_memory, pa) & ~mask);
}
static uint16_t vring_used_idx(VirtQueue *vq)
{
    hwaddr pa;
    pa = vq->vring.used + offsetof(VRingUsed, idx);
    return lduw_phys(&address_space_memory, pa);
}
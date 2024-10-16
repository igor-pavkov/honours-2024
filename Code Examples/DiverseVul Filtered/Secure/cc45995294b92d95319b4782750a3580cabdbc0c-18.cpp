static inline uint16_t vring_desc_next(hwaddr desc_pa, int i)
{
    hwaddr pa;
    pa = desc_pa + sizeof(VRingDesc) * i + offsetof(VRingDesc, next);
    return lduw_phys(&address_space_memory, pa);
}
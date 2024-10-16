static inline uint16_t vring_desc_flags(hwaddr desc_pa, int i)
{
    hwaddr pa;
    pa = desc_pa + sizeof(VRingDesc) * i + offsetof(VRingDesc, flags);
    return lduw_phys(&address_space_memory, pa);
}
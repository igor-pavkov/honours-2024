static inline uint32_t vring_desc_len(hwaddr desc_pa, int i)
{
    hwaddr pa;
    pa = desc_pa + sizeof(VRingDesc) * i + offsetof(VRingDesc, len);
    return ldl_phys(&address_space_memory, pa);
}
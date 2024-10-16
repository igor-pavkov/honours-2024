static inline uint64_t vring_desc_addr(hwaddr desc_pa, int i)
{
    hwaddr pa;
    pa = desc_pa + sizeof(VRingDesc) * i + offsetof(VRingDesc, addr);
    return ldq_phys(&address_space_memory, pa);
}
static hwaddr intel_hda_addr(uint32_t lbase, uint32_t ubase)
{
    return ((uint64_t)ubase << 32) | lbase;
}
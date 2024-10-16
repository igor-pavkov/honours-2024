static uint64_t vga_mem_read(void *opaque, hwaddr addr,
                             unsigned size)
{
    VGACommonState *s = opaque;

    return vga_mem_readb(s, addr);
}
void vbe_ioport_write_index(void *opaque, uint32_t addr, uint32_t val)
{
    VGACommonState *s = opaque;
    s->vbe_index = val;
}
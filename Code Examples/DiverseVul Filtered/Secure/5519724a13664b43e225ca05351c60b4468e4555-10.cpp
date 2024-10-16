static uint64_t enet_read(void *opaque, hwaddr addr, unsigned size)
{
    XgmacState *s = opaque;
    uint64_t r = 0;
    addr >>= 2;

    switch (addr) {
    case XGMAC_VERSION:
        r = 0x1012;
        break;
    default:
        if (addr < ARRAY_SIZE(s->regs)) {
            r = s->regs[addr];
        }
        break;
    }
    return r;
}
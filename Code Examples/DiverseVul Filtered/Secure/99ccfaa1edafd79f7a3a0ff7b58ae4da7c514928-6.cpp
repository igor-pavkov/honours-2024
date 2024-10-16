void pcnet_ioport_writel(void *opaque, uint32_t addr, uint32_t val)
{
    PCNetState *s = opaque;
    pcnet_poll_timer(s);
#ifdef PCNET_DEBUG_IO
    printf("pcnet_ioport_writel addr=0x%08x val=0x%08x\n", addr, val);
#endif
    if (BCR_DWIO(s)) {
        switch (addr & 0x0f) {
        case 0x00: /* RDP */
            pcnet_csr_writew(s, s->rap, val & 0xffff);
            break;
        case 0x04:
            s->rap = val & 0x7f;
            break;
        case 0x0c:
            pcnet_bcr_writew(s, s->rap, val & 0xffff);
            break;
        }
    } else if ((addr & 0x0f) == 0) {
        /* switch device to dword i/o mode */
        pcnet_bcr_writew(s, BCR_BSBC, pcnet_bcr_readw(s, BCR_BSBC) | 0x0080);
#ifdef PCNET_DEBUG_IO
        printf("device switched into dword i/o mode\n");
#endif
    }
    pcnet_update_irq(s);
}
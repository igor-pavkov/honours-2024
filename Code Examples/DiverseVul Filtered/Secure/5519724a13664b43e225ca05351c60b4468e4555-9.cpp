static void xgmac_write_desc(XgmacState *s, struct desc *d, int rx)
{
    int reg = rx ? DMA_CUR_RX_DESC_ADDR : DMA_CUR_TX_DESC_ADDR;
    uint32_t addr = s->regs[reg];

    if (!rx && (d->ctl_stat & 0x00200000)) {
        s->regs[reg] = s->regs[DMA_TX_BASE_ADDR];
    } else if (rx && (d->buffer1_size & 0x8000)) {
        s->regs[reg] = s->regs[DMA_RCV_BASE_ADDR];
    } else {
        s->regs[reg] += sizeof(*d);
    }
    cpu_physical_memory_write(addr, d, sizeof(*d));
}
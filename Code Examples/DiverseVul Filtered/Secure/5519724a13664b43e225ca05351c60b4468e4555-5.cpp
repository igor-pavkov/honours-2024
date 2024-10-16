static void xgmac_read_desc(XgmacState *s, struct desc *d, int rx)
{
    uint32_t addr = rx ? s->regs[DMA_CUR_RX_DESC_ADDR] :
        s->regs[DMA_CUR_TX_DESC_ADDR];
    cpu_physical_memory_read(addr, d, sizeof(*d));
}
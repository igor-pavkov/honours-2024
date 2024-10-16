static void enet_write(void *opaque, hwaddr addr,
                       uint64_t value, unsigned size)
{
    XgmacState *s = opaque;

    addr >>= 2;
    switch (addr) {
    case DMA_BUS_MODE:
        s->regs[DMA_BUS_MODE] = value & ~0x1;
        break;
    case DMA_XMT_POLL_DEMAND:
        xgmac_enet_send(s);
        break;
    case DMA_STATUS:
        s->regs[DMA_STATUS] = s->regs[DMA_STATUS] & ~value;
        break;
    case DMA_RCV_BASE_ADDR:
        s->regs[DMA_RCV_BASE_ADDR] = s->regs[DMA_CUR_RX_DESC_ADDR] = value;
        break;
    case DMA_TX_BASE_ADDR:
        s->regs[DMA_TX_BASE_ADDR] = s->regs[DMA_CUR_TX_DESC_ADDR] = value;
        break;
    default:
        if (addr < ARRAY_SIZE(s->regs)) {
            s->regs[addr] = value;
        }
        break;
    }
    enet_update_irq(s);
}
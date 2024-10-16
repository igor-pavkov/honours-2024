static ssize_t eth_rx(NetClientState *nc, const uint8_t *buf, size_t size)
{
    XgmacState *s = qemu_get_nic_opaque(nc);
    static const unsigned char sa_bcast[6] = {0xff, 0xff, 0xff,
                                              0xff, 0xff, 0xff};
    int unicast, broadcast, multicast;
    struct desc bd;
    ssize_t ret;

    if (!eth_can_rx(s)) {
        return -1;
    }
    unicast = ~buf[0] & 0x1;
    broadcast = memcmp(buf, sa_bcast, 6) == 0;
    multicast = !unicast && !broadcast;
    if (size < 12) {
        s->regs[DMA_STATUS] |= DMA_STATUS_RI | DMA_STATUS_NIS;
        ret = -1;
        goto out;
    }

    xgmac_read_desc(s, &bd, 1);
    if ((bd.ctl_stat & 0x80000000) == 0) {
        s->regs[DMA_STATUS] |= DMA_STATUS_RU | DMA_STATUS_AIS;
        ret = size;
        goto out;
    }

    cpu_physical_memory_write(bd.buffer1_addr, buf, size);

    /* Add in the 4 bytes for crc (the real hw returns length incl crc) */
    size += 4;
    bd.ctl_stat = (size << 16) | 0x300;
    xgmac_write_desc(s, &bd, 1);

    s->stats.rx_bytes += size;
    s->stats.rx++;
    if (multicast) {
        s->stats.rx_mcast++;
    } else if (broadcast) {
        s->stats.rx_bcast++;
    }

    s->regs[DMA_STATUS] |= DMA_STATUS_RI | DMA_STATUS_NIS;
    ret = size;

out:
    enet_update_irq(s);
    return ret;
}
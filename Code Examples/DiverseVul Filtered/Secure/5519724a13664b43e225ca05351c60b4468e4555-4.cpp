static void xgmac_enet_realize(DeviceState *dev, Error **errp)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    XgmacState *s = XGMAC(dev);

    memory_region_init_io(&s->iomem, OBJECT(s), &enet_mem_ops, s,
                          "xgmac", 0x1000);
    sysbus_init_mmio(sbd, &s->iomem);
    sysbus_init_irq(sbd, &s->sbd_irq);
    sysbus_init_irq(sbd, &s->pmt_irq);
    sysbus_init_irq(sbd, &s->mci_irq);

    qemu_macaddr_default_if_unset(&s->conf.macaddr);
    s->nic = qemu_new_nic(&net_xgmac_enet_info, &s->conf,
                          object_get_typename(OBJECT(dev)), dev->id, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);

    s->regs[XGMAC_ADDR_HIGH(0)] = (s->conf.macaddr.a[5] << 8) |
                                   s->conf.macaddr.a[4];
    s->regs[XGMAC_ADDR_LOW(0)] = (s->conf.macaddr.a[3] << 24) |
                                 (s->conf.macaddr.a[2] << 16) |
                                 (s->conf.macaddr.a[1] << 8) |
                                  s->conf.macaddr.a[0];
}
static void lan9118_realize(DeviceState *dev, Error **errp)
{
    SysBusDevice *sbd = SYS_BUS_DEVICE(dev);
    lan9118_state *s = LAN9118(dev);
    int i;
    const MemoryRegionOps *mem_ops =
            s->mode_16bit ? &lan9118_16bit_mem_ops : &lan9118_mem_ops;

    memory_region_init_io(&s->mmio, OBJECT(dev), mem_ops, s,
                          "lan9118-mmio", 0x100);
    sysbus_init_mmio(sbd, &s->mmio);
    sysbus_init_irq(sbd, &s->irq);
    qemu_macaddr_default_if_unset(&s->conf.macaddr);

    s->nic = qemu_new_nic(&net_lan9118_info, &s->conf,
                          object_get_typename(OBJECT(dev)), dev->id, s);
    qemu_format_nic_info_str(qemu_get_queue(s->nic), s->conf.macaddr.a);
    s->eeprom[0] = 0xa5;
    for (i = 0; i < 6; i++) {
        s->eeprom[i + 1] = s->conf.macaddr.a[i];
    }
    s->pmt_ctrl = 1;
    s->txp = &s->tx_packet;

    s->timer = ptimer_init(lan9118_tick, s, PTIMER_POLICY_DEFAULT);
    ptimer_transaction_begin(s->timer);
    ptimer_set_freq(s->timer, 10000);
    ptimer_set_limit(s->timer, 0xffff, 1);
    ptimer_transaction_commit(s->timer);
}
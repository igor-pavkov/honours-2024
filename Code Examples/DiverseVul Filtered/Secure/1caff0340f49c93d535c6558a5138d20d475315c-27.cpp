static void e1000_reset(void *opaque)
{
    E1000State *d = opaque;
    E1000BaseClass *edc = E1000_GET_CLASS(d);
    uint8_t *macaddr = d->conf.macaddr.a;

    timer_del(d->autoneg_timer);
    timer_del(d->mit_timer);
    timer_del(d->flush_queue_timer);
    d->mit_timer_on = 0;
    d->mit_irq_level = 0;
    d->mit_ide = 0;
    memset(d->phy_reg, 0, sizeof d->phy_reg);
    memmove(d->phy_reg, phy_reg_init, sizeof phy_reg_init);
    d->phy_reg[PHY_ID2] = edc->phy_id2;
    memset(d->mac_reg, 0, sizeof d->mac_reg);
    memmove(d->mac_reg, mac_reg_init, sizeof mac_reg_init);
    d->rxbuf_min_shift = 1;
    memset(&d->tx, 0, sizeof d->tx);

    if (qemu_get_queue(d->nic)->link_down) {
        e1000x_update_regs_on_link_down(d->mac_reg, d->phy_reg);
    }

    e1000x_reset_mac_addr(d->nic, d->mac_reg, macaddr);
}
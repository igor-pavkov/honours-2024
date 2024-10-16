static void phy_update_link(lan9118_state *s)
{
    /* Autonegotiation status mirrors link status.  */
    if (qemu_get_queue(s->nic)->link_down) {
        s->phy_status &= ~0x0024;
        s->phy_int |= PHY_INT_DOWN;
    } else {
        s->phy_status |= 0x0024;
        s->phy_int |= PHY_INT_ENERGYON;
        s->phy_int |= PHY_INT_AUTONEG_COMPLETE;
    }
    phy_update_irq(s);
}
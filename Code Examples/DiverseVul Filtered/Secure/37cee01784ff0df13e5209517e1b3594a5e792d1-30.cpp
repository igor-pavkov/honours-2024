static void phy_update_irq(lan9118_state *s)
{
    if (s->phy_int & s->phy_int_mask) {
        s->int_sts |= PHY_INT;
    } else {
        s->int_sts &= ~PHY_INT;
    }
    lan9118_update(s);
}
static void phy_reset(lan9118_state *s)
{
    s->phy_status = 0x7809;
    s->phy_control = 0x3000;
    s->phy_advertise = 0x01e1;
    s->phy_int_mask = 0;
    s->phy_int = 0;
    phy_update_link(s);
}
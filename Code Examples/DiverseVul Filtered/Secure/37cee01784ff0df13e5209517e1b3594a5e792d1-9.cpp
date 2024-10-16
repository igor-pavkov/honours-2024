static uint32_t do_phy_read(lan9118_state *s, int reg)
{
    uint32_t val;

    switch (reg) {
    case 0: /* Basic Control */
        return s->phy_control;
    case 1: /* Basic Status */
        return s->phy_status;
    case 2: /* ID1 */
        return 0x0007;
    case 3: /* ID2 */
        return 0xc0d1;
    case 4: /* Auto-neg advertisement */
        return s->phy_advertise;
    case 5: /* Auto-neg Link Partner Ability */
        return 0x0f71;
    case 6: /* Auto-neg Expansion */
        return 1;
        /* TODO 17, 18, 27, 29, 30, 31 */
    case 29: /* Interrupt source.  */
        val = s->phy_int;
        s->phy_int = 0;
        phy_update_irq(s);
        return val;
    case 30: /* Interrupt mask */
        return s->phy_int_mask;
    default:
        BADF("PHY read reg %d\n", reg);
        return 0;
    }
}
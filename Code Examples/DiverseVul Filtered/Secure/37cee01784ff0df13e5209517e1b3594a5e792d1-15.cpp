static void do_phy_write(lan9118_state *s, int reg, uint32_t val)
{
    switch (reg) {
    case 0: /* Basic Control */
        if (val & 0x8000) {
            phy_reset(s);
            break;
        }
        s->phy_control = val & 0x7980;
        /* Complete autonegotiation immediately.  */
        if (val & 0x1000) {
            s->phy_status |= 0x0020;
        }
        break;
    case 4: /* Auto-neg advertisement */
        s->phy_advertise = (val & 0x2d7f) | 0x80;
        break;
        /* TODO 17, 18, 27, 31 */
    case 30: /* Interrupt mask */
        s->phy_int_mask = val & 0xff;
        phy_update_irq(s);
        break;
    default:
        BADF("PHY write reg %d = 0x%04x\n", reg, val);
    }
}
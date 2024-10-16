static void lan9118_eeprom_cmd(lan9118_state *s, int cmd, int addr)
{
    s->e2p_cmd = (s->e2p_cmd & E2P_CMD_MAC_ADDR_LOADED) | (cmd << 28) | addr;
    switch (cmd) {
    case 0:
        s->e2p_data = s->eeprom[addr];
        DPRINTF("EEPROM Read %d = 0x%02x\n", addr, s->e2p_data);
        break;
    case 1:
        s->eeprom_writable = 0;
        DPRINTF("EEPROM Write Disable\n");
        break;
    case 2: /* EWEN */
        s->eeprom_writable = 1;
        DPRINTF("EEPROM Write Enable\n");
        break;
    case 3: /* WRITE */
        if (s->eeprom_writable) {
            s->eeprom[addr] &= s->e2p_data;
            DPRINTF("EEPROM Write %d = 0x%02x\n", addr, s->e2p_data);
        } else {
            DPRINTF("EEPROM Write %d (ignored)\n", addr);
        }
        break;
    case 4: /* WRAL */
        if (s->eeprom_writable) {
            for (addr = 0; addr < 128; addr++) {
                s->eeprom[addr] &= s->e2p_data;
            }
            DPRINTF("EEPROM Write All 0x%02x\n", s->e2p_data);
        } else {
            DPRINTF("EEPROM Write All (ignored)\n");
        }
        break;
    case 5: /* ERASE */
        if (s->eeprom_writable) {
            s->eeprom[addr] = 0xff;
            DPRINTF("EEPROM Erase %d\n", addr);
        } else {
            DPRINTF("EEPROM Erase %d (ignored)\n", addr);
        }
        break;
    case 6: /* ERAL */
        if (s->eeprom_writable) {
            memset(s->eeprom, 0xff, 128);
            DPRINTF("EEPROM Erase All\n");
        } else {
            DPRINTF("EEPROM Erase All (ignored)\n");
        }
        break;
    case 7: /* RELOAD */
        lan9118_reload_eeprom(s);
        break;
    }
}
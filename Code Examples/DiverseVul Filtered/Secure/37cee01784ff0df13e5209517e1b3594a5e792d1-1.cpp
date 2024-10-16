static void lan9118_reload_eeprom(lan9118_state *s)
{
    int i;
    if (s->eeprom[0] != 0xa5) {
        s->e2p_cmd &= ~E2P_CMD_MAC_ADDR_LOADED;
        DPRINTF("MACADDR load failed\n");
        return;
    }
    for (i = 0; i < 6; i++) {
        s->conf.macaddr.a[i] = s->eeprom[i + 1];
    }
    s->e2p_cmd |= E2P_CMD_MAC_ADDR_LOADED;
    DPRINTF("MACADDR loaded from eeprom\n");
    lan9118_mac_changed(s);
}
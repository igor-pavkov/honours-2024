static uint32_t rtl8139_TxConfig_read(RTL8139State *s)
{
    uint32_t ret = s->TxConfig;

    DPRINTF("TxConfig read val=0x%04x\n", ret);

    return ret;
}
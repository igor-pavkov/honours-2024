static char *mac_strdup_printf(const uint8_t *mac)
{
    return g_strdup_printf("%.2x:%.2x:%.2x:%.2x:%.2x:%.2x", mac[0],
                            mac[1], mac[2], mac[3], mac[4], mac[5]);
}
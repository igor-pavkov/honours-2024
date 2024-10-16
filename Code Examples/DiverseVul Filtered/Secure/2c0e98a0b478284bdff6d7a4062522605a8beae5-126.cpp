static void param_write_hex(AVIOContext *pb, const char *name, const uint8_t *value, int len)
{
    char buf[150];
    len = FFMIN(sizeof(buf) / 2 - 1, len);
    ff_data_to_hex(buf, value, len, 0);
    buf[2 * len] = '\0';
    avio_printf(pb, "<param name=\"%s\" value=\"%s\" valuetype=\"data\"/>\n", name, buf);
}
static void param_write_string(AVIOContext *pb, const char *name, const char *value)
{
    avio_printf(pb, "<param name=\"%s\" value=\"%s\" valuetype=\"data\"/>\n", name, value);
}
static void build_xref(char *msgid, char *buf, size_t size, int body_only)
{
    struct xref_rock xrock = { buf, size };

    snprintf(buf, size, "%s%s", body_only ? "" : "Xref: ", config_servername);
    duplicate_find(msgid, &xref_cb, &xrock);
}
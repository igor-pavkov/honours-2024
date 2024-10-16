void udscs_write(UdscsConnection *conn, uint32_t type, uint32_t arg1,
    uint32_t arg2, const uint8_t *data, uint32_t size)
{
    gpointer buf;
    guint buf_size;
    struct udscs_message_header header;

    buf_size = sizeof(header) + size;
    buf = g_malloc(buf_size);

    header.type = type;
    header.arg1 = arg1;
    header.arg2 = arg2;
    header.size = size;

    memcpy(buf, &header, sizeof(header));
    memcpy(buf + sizeof(header), data, size);

    debug_print_message_header(conn, &header, "sent");

    vdagent_connection_write(VDAGENT_CONNECTION(conn), buf, buf_size);
}